import json
import os
import sys
import struct
import subprocess

DEBUG_MODE = False

OPCODE_PREFIX = 42
OPCODE_VERSION = 1

OPCODE_BTX_RESULT = 4
OPCODE_BTX_RESULT_PAYOUT = 1
OPCODE_BTX_RESULT_EVENT_REFUND = 2
OPCODE_BTX_RESULT_MONEYLINE_REFUND = 3

OPCODE_BTX_MONEYLINE = 5
OPCODE_BTX_SPREAD = 9
OPCODE_BTX_TOTAL = 10

WGR_TX_FEE = 0.001
WGR_WALLET_ADDRESS = {
    "mainnet": "Weqz3PFBq3SniYF5HS8kuj72q9FABKzDrP",
    "testnet": "TLceyDrdPLBu8DK6UZjKu4vCDUQBGPybc",
    "regtest": "TFvZVYGdrxxNunQLzSnRSC58BSRA7si6zu",
    }
WGR_NETWORK = "mainnet"
WAGERR_CLI_PATH = os.path.join("wagerr-cli")



# Encode an unsigned int in hexadecimal and little endian byte order. The function expects the value and the size in
# bytes as parameters.
def encode_int_little_endian(value: int, size: int):
    if size == 1:
        return struct.pack('<B', int(value)).hex()
    elif size == 2:
        return struct.pack('<H', int(value)).hex()
    elif size == 4:
        return struct.pack('<I', int(value)).hex()
    else:
        print("events", "Incorrect byte size was specified for encoding.")


# Encode a string in hexadecimal notation. Byte order does not matter for UTF-8 encoded strings, however, I have kept
# used similar encoding methods as I would to encode in little-endian byte order to keep things consistent with int
# handling. The end result for a string is the same for little/big endian.
#
# There is no limit to the size of the encoded string as the function will use the length of the string.
def encode_str_hex(value: str):
    value = bytes(value.encode('utf8'))
    return struct.pack("<%ds" % (len(value)), value).hex()


# Create a result opcode.
def result_opcode():
    # Ask for user input.
    event_id = int(input("Event ID: "))
    home_score = float(input("Home Score: "))
    home_score = int(home_score * 10)
    print(home_score)
    away_score = float(input("Away Score: "))
    away_score = int(away_score * 10)
    print(away_score)

    # Encode
    prefix = str(OPCODE_PREFIX)
    version_hex = str(encode_int_little_endian(OPCODE_VERSION, 1))
    btx_type_hex = str(encode_int_little_endian(OPCODE_BTX_RESULT, 1))
    event_id_hex = str(encode_int_little_endian(event_id, 4))

    result_type = int(input("Result type (1. Payout - 2. Event Refund - 3. Moneyline Refund): "))

    while result_type > 3 or result_type < 1:
        print("Error: Input must be between 1 and 3.")
        result_type = int(input("Result type (1. Payout - 2. Event Refund - 3. Moneyline Refund): "))

    if result_type == 1:
        result_type_hex = str(encode_int_little_endian(OPCODE_BTX_RESULT_PAYOUT, 1))
    elif result_type == 2:
        result_type_hex = str(encode_int_little_endian(OPCODE_BTX_RESULT_EVENT_REFUND, 1))
    elif result_type == 3:
        result_type_hex = str(encode_int_little_endian(OPCODE_BTX_RESULT_MONEYLINE_REFUND, 1))

    home_team_score_hex = str(encode_int_little_endian(home_score, 2))
    away_team_score_hex = str(encode_int_little_endian(away_score, 2))

    opcode = prefix + version_hex + btx_type_hex + event_id_hex + result_type_hex + home_team_score_hex + away_team_score_hex

    print("\nOpcode: {}".format(opcode))

    return opcode


# Create a moneyline opcode.
def moneyline_opcode():
    # Ask for user input.
    event_id = int(input("Event ID: "))
    home_odds = float(input("Home Odds: "))
    home_odds = int(home_odds * 10000)
    print(home_odds)
    away_odds = float(input("Away Odds: "))
    away_odds = int(away_odds * 10000)
    print(away_odds)
    draw_odds = float(input("Away Odds: "))
    draw_odds = int(draw_odds * 10000)
    print(draw_odds)

    # Encode
    prefix = str(OPCODE_PREFIX)
    version_hex = str(encode_int_little_endian(OPCODE_VERSION, 1))
    btx_type_hex = str(encode_int_little_endian(OPCODE_BTX_MONEYLINE, 1))
    event_id_hex = str(encode_int_little_endian(event_id, 4))
    home_odds_hex = str(encode_int_little_endian(home_odds, 4))
    away_odds_hex = str(encode_int_little_endian(away_odds, 4))
    draw_odds_hex = str(encode_int_little_endian(draw_odds, 4))

    opcode = prefix + version_hex + btx_type_hex + event_id_hex + home_odds_hex + away_odds_hex + draw_odds_hex

    print("\nOpcode: {}".format(opcode))

    return opcode


# Create a spread opcode.
def spread_opcode():
    # Ask for user input.
    event_id = int(input("Event ID: "))
    spread_points = float(input("Spread Points: "))
    spread_points = int(spread_points * 10)
    print(spread_points)
    home_odds = float(input("Home Odds: "))
    home_odds = int(home_odds * 10000)
    print(home_odds)
    away_odds = float(input("Away Odds: "))
    away_odds = int(away_odds * 10000)
    print(away_odds)

    # Encode
    prefix = str(OPCODE_PREFIX)
    version_hex = str(encode_int_little_endian(OPCODE_VERSION, 1))
    btx_type_hex = str(encode_int_little_endian(OPCODE_BTX_SPREAD, 1))
    event_id_hex = str(encode_int_little_endian(event_id, 4))
    spread_points_hex = str(encode_int_little_endian(spread_points, 2))
    home_odds_hex = str(encode_int_little_endian(home_odds, 4))
    away_odds_hex = str(encode_int_little_endian(away_odds, 4))

    opcode = prefix + version_hex + btx_type_hex + event_id_hex + spread_points_hex + home_odds_hex + away_odds_hex

    print("\nOpcode: {}".format(opcode))

    return opcode


# Create a total opcode.
def total_opcode():
    # Ask for user input.
    event_id = int(input("Event ID: "))
    total_points = float(input("Total Points: "))
    total_points = int(total_points * 10)
    print(total_points)
    over_odds = float(input("Over Odds: "))
    over_odds = int(over_odds * 10000)
    print(over_odds)
    under_odds = float(input("Under Odds: "))
    under_odds = int(under_odds * 10000)
    print(under_odds)

    # Encode
    prefix = str(OPCODE_PREFIX)
    version_hex = str(encode_int_little_endian(OPCODE_VERSION, 1))
    btx_type_hex = str(encode_int_little_endian(OPCODE_BTX_TOTAL, 1))
    event_id_hex = str(encode_int_little_endian(event_id, 4))
    total_points_hex = str(encode_int_little_endian(total_points, 2))
    over_odds_hex = str(encode_int_little_endian(over_odds, 4))
    under_odds_hex = str(encode_int_little_endian(under_odds, 4))

    opcode = prefix + version_hex + btx_type_hex + event_id_hex + total_points_hex + over_odds_hex + under_odds_hex

    print("\nOpcode: {}".format(opcode))

    return opcode


def post_opcode(opcode: str):
    try:
        # Get unspent outputs to use as inputs (spend).
        inputs, spend = get_utxo_list()

        # Calculate the change by subtracting the transaction fee from the UTXO's value.
        change = str(float(spend) - float(WGR_TX_FEE))

        # Create the output JSON
        outputs = "{\"" + WGR_WALLET_ADDRESS[WGR_NETWORK] + "\":" + change + ",\"""data\":\"" + opcode + "\"}"

        # Create the raw transaction.
        create_tx_response = subprocess.check_output([WAGERR_CLI_PATH, "-"+WGR_NETWORK, "createrawtransaction", inputs, outputs]).rstrip()
        create_tx_response = create_tx_response.decode('utf-8')
        print("Raw Transaction: " + create_tx_response)

        # Sign the raw transaction.
        sign_tx_response = subprocess.check_output([WAGERR_CLI_PATH, "-"+WGR_NETWORK, "signrawtransaction", create_tx_response])
        sign_tx_response = sign_tx_response.decode('utf-8')
        sign_tx_data = json.loads(sign_tx_response)
        signed_tx_hex = sign_tx_data['hex']
        print("Signed Transaction: " + signed_tx_hex)

        # Send the signed transaction to the Wagerr network.
        if DEBUG_MODE is True:
            print("Not posting transaction for opcode {} while in debug mode.".format(opcode))
        else:
            send_tx_response = subprocess.check_output([WAGERR_CLI_PATH, "-"+WGR_NETWORK, "sendrawtransaction", signed_tx_hex])
            print("Successfully sent transaction for opcode {}: {}".format(opcode, send_tx_response))

    except Exception as e:
        print("Error: Could not create transaction to post opcode {}.".format(opcode))
        print(e)


def get_utxo_list():
    total_amount = float(0.00)
    min_amount = WGR_TX_FEE

    # Get a list of unspent transactions (UTXO)
    try:
        utxo_response = subprocess.check_output([WAGERR_CLI_PATH, "-"+WGR_NETWORK, "listunspent", 1, 9999999, [WGR_WALLET_ADDRESS[WGR_NETWORK]]])
        utxo_data = json.loads(utxo_response)
    except Exception as e:
        print("Error: Can't connect to wagerr-cli daemon.")
        print(e)
        return None

    # Find enough UTXO to use in a spend transaction to cover the minimum amount.
    utxo_array = []
    for utxo in utxo_data:
            utxo_array.append({"txid": utxo["txid"], "vout": utxo["vout"]})
            total_amount += float(utxo['amount'])

            if total_amount > min_amount:
                return json.dumps(utxo_array), str(total_amount)


def generate_opcode():
    # Ask for opcode type (result or event).
    opcode_type = int(input("Opcode type (1. Result - 2. Moneyline Update - 3. Spreads Update - 4. Totals Update): "))

    while opcode_type > 4 or opcode_type < 1:
        print("Error: Input must be between 1 and 3.")
        opcode_type = int(input("Opcode type (1. Result - 2. Moneyline Update - 3. Spreads Update - 4. Totals Update): "))

    # Create the opcode depending on user input.
    if opcode_type == 1:
        opcode = result_opcode()
    elif opcode_type == 2:
        opcode = moneyline_opcode()
    elif opcode_type == 3:
        opcode = spread_opcode()
    elif opcode_type == 4:
        opcode = total_opcode()

    return opcode

if __name__ == '__main__':
    print("OPCODE BUILDER")
    print("==============\n")
    if len(sys.argv) > 1:
        WGR_NETWORK = str(sys.argv[1])
        if WGR_NETWORK != "mainnet" or WGR_NETWORK != "testnet" or WGR_NETWORK != "regtest":
            print("Invalid chain network! Must be once of mainnet, testnet, regtest.\nUsage: python3 "+sys.argv[0]+" mainnet/testnet/regtest")
            exit(1)
    opcode = generate_opcode()
    post_opcode(opcode)