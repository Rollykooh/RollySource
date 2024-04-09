import requests
import hmac
import hashlib
import json
import time

api_key = 'your_api_key'
api_secret = 'your_api_secret'

base_url = 'https://api.binance.com/api/v3'

# available symbol in binance

# BTCUSDT
# ETHBTC
# XRPUSDT
# ADABTC
# LTCUSDT

##############################################

def get_server_time():
    response = requests.get('https://api.binance.com/api/v3/time')
    return response.json()

def get_account_balance():
    timestamp = int(time.time() * 1000)
    params = { 'timestamp': timestamp}
    params['recvWindow'] = 5000
    query_string = '&'.join([f"{k}={v}" for k, v in params.items()])
    signature = hmac.new(api_secret.encode(), query_string.encode(), hashlib.sha256).hexdigest()
    
    headers = {
        'X-MBX-APIKEY': api_key
    }
    url = 'https://api.binance.com/api/v3/account'+ '?' + query_string + f'&signature={signature}'
    
    response = requests.get(url, headers=headers)
    return response.json()

def bring_balances():
    headers = {
        'X-MBX-APIKEY': api_key
    }

    response = requests.get(f'https://api.binance.com/api/v3/account', 
        headers=headers, params={'timestamp': int(time.time()) * 1000, 'signature': hmac.new(api_secret.encode(), ''.encode(), hashlib.sha256).hexdigest()}).json()

    balances = {}
    for balance in response['balances']:
        asset = balance['asset']
        free = float(balance['free'])
        locked = float(balance['locked'])
        total = free + locked
        if total > 0:
            balances[asset] = {'free': free, 'locked': locked}
    
    return balances

def get_position(symbol):  
    headers = {
        'X-MBX-APIKEY': api_key
    }
    
    response = requests.get('https://api.binance.com/api/v3/account', headers=headers)
    
    if response.status_code == 200:
        data = response.json()
        positions = data['balances']
        
        for position in positions:
            if position['asset'] == symbol:
                return float(position['free'])     
    return 0

def get_market_symbol(symbol):
    params = {'symbol' : symbol}
    response = requests.get('https://api.binance.com/api/v3/ticker/price' , params=params)
    return response.json()

def get_market_exchange_information():
    response = requests.get('https://api.binance.com/api/v3/exchangeInfo')

    #curl -X GET "https://api.binance.com/api/v3/exchangeInfo?symbol=BNBBTC"
    #curl -g -X GET 'https://api.binance.com/api/v3/exchangeInfo?symbols=["BTCUSDT","BNBBTC"]'
    #curl -X GET "https://api.binance.com/api/v3/exchangeInfo?permissions=SPOT"

    return response.json()

def get_market_order_book(symbol, limit=5):
    params = {'symbol':symbol,'limit':limit}
    response = requests.get('https://api.binance.com/api/v3/depth',params=params)
    if(response.status_code==200):
        return response.json()
    else:
        return 'Error getting order book: {response.text}'

def get_market_recent_trades_list(symbol):
    params = {'symbol':symbol,'limit':10}
    response = requests.get('https://api.binance.com/api/v3/trades',params=params)
    return response.json()

def get_market_current_average_price(symbol):
    params={'symbol':symbol}
    response = requests.get('https://api.binance.com/api/v3/avgPrice',params=params)
    return response.json()

def get_symbol_lastestprice(symbol):
    url = f"https://api.binance.com/api/v3/ticker/price?symbol={symbol}"

    response = requests.get(url)
    data = response.json()

    latest_price = data['price']
    return latest_price

def get_min_max_price(symbol):
    params = {'symbol' : symbol}
    response = requests.get('https://api.binance.com/api/v3/ticker/24hr' , params=params)

    if response.status_code == 200:
        data = response.json()
        return {"symbol": symbol, "min_price": data["lowPrice"], "max_price": data["highPrice"]}
    else:
        return {"error": "Unable to fetch data from Binance API"}


def post_order(params):
    query_string = '&'.join([f"{k}={params[k]}" for k in sorted(params.keys())])
    signature = hmac.new(api_secret.encode('utf-8'), query_string.encode('utf-8'), hashlib.sha256).hexdigest()

    headers = {
        'X-MBX-APIKEY': api_key
    }

    params['signature'] = signature

    response = requests.post('https://api.binance.com/api/v3/order', headers=headers, params=params)
    return response.json()


# Get Server Time

stime = get_server_time()
print(int(time.time() * 1000)-stime['serverTime'])

# Get account balances
balances = get_account_balance()
print(balances)

# Get position

symbol = 'BTC'  # The symbol of the coin you want to get the position of
position = get_position(symbol)

print(f'Position of {symbol} in Binance: {position}')

# Get market data

market_data = get_market_symbol('BTCUSDT')
print(market_data)

exchange_information = get_market_exchange_information()
print(exchange_information)

order_book = get_market_order_book('BTCUSDT')
print(order_book)

recent_trades_list = get_market_recent_trades_list('BTCUSDT')
print(recent_trades_list)

current_average_price = get_market_current_average_price('BTCUSDT')
print(current_average_price)

min_max_price = get_min_max_price('BTCUSDT')
print(min_max_price)

# Post order
params = {
    'symbol': 'BTCUSDT',
    'side': 'BUY',  #'SELL'
    'type': 'MARKET',   #'LIMIT'
    'timeInForce' : 'GTC',
    'quantity': 0.01,
    'price' : 50000,
    'timestamp' : int(time.time()*1000)
}

order = post_order(params)
print(order)