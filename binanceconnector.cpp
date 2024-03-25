#include "binanceconnector.h"

#include <iostream>
#include <stdio.h>
#include <string>

#include "nlohmann-json/single_include/nlohmann/json.hpp"

#include "cryptopp890/hmac.h"
#include "cryptopp890/cryptlib.h"
#include "cryptopp890/sha.h"
#include "cryptopp890/filters.h"
#include "cryptopp890/hex.h"

using json = nlohmann::json;
using namespace std;

string api_key = "your_api_key";
string api_secret = "your_api_secret";

std::string calculateHMAC(const std::string& key, const std::string& message) {
	CryptoPP::HMAC<CryptoPP::SHA256> hmac((const unsigned char*)key.data(), key.size());

	std::string mac;
	CryptoPP::StringSource(message, true,
		new CryptoPP::HashFilter(hmac,
			new CryptoPP::HexEncoder(
				new CryptoPP::StringSink(mac)
			)
		)
	);

	return mac;
}


std::string getDate(long long utcTime)
{
	std::string time;
	tm timeinfo;
	char buffer[80] = { NULL };

	utcTime /= 1000;

	gmtime_s(&timeinfo, &utcTime);

	strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", &timeinfo);

	time = buffer;

	return time;
}


string GET(string url)
{
	char buffer[2];

	std::string body;

	std::string cmd = "curl -s " + url;

	FILE* p = _popen(cmd.c_str(), "r");

	if (!p)
	{
		return "curl request error";
	}

	while (fgets(buffer, sizeof(buffer), p))
	{
		body += buffer;
	}
	return body;
}

void get_available_symbols()
{
	string url = "https://api.binance.com/api/v1/exchangeInfo";

	string response = GET(url);

	auto jsonResponse = json::parse(response);

	printf("------------------------------get_available_symbols-------------------------------\n\n");

	for (const auto& symbol : jsonResponse["symbols"])
	{
		string sym = symbol["symbol"];
		printf("%s, ", sym.c_str());
	}

	printf("\n");
	printf("----------------------------------------------------------------------------------\n\n");
}


////////////////////////////////////////////////////////////////////////////////////////////

bool check_binance_network()
{
	string url = "https://api.binance.com/api/v3/ping";

	string response = GET(url);

	if (response.compare("{}"))
		return false;
	
	return true;
}


void get_server_time()
{
	string url = "https://api.binance.com/api/v3/time";

	string response = GET(url);

	auto jsonResponse = json::parse(response);
	long long servertime;
	string time;

	if (!jsonResponse["serverTime"].empty())
	{
		servertime = jsonResponse["serverTime"];

		time = getDate(servertime);

		printf("---------------------------------get_server_time----------------------------------\n\n");

		printf("	serverTime : %s\n", time.c_str());

		printf("----------------------------------------------------------------------------------\n\n");
	}

}


void get_market_symbol(string symbol) {
	string url = "https://api.binance.com/api/v3/ticker/price?symbol=" + symbol;

	string response = GET(url);

	auto jsonResponse = json::parse(response);

	if (!jsonResponse["symbol"].empty())
	{
		string sym = jsonResponse["symbol"];
		string price = jsonResponse["price"];

		printf("--------------------------------get_market_symbol---------------------------------\n\n");

		printf("	symbol : %s, price : %s\n", sym.c_str(),price.c_str());

		printf("----------------------------------------------------------------------------------\n\n");
	}

}


void get_market_exchange_information(string symbol)
{
	string url = "https://api.binance.com/api/v3/exchangeInfo?symbol=" + symbol;
	string response = GET(url);

	auto jsonResponse = json::parse(response);

	//curl -X GET "https://api.binance.com/api/v3/exchangeInfo"
	//curl - X GET "https://api.binance.com/api/v3/exchangeInfo?symbol=BNBBTC"
	//curl - g - X GET "https://api.binance.com/api/v3/exchangeInfo?symbols=["BTCUSDT","BNBBTC"]"
	//curl - X GET "https://api.binance.com/api/v3/exchangeInfo?permissions=SPOT"

	printf("-------------------------get_market_exchange_information--------------------------\n\n");

	/*
	{"timezone":"UTC","serverTime":1711272981650,
	"rateLimits":
	[
	{"rateLimitType":"REQUEST_WEIGHT","interval":"MINUTE","intervalNum":1,"limit":6000},
	{"rateLimitType":"ORDERS","interval":"SECOND","intervalNum":10,"limit":100},
	{"rateLimitType":"ORDERS","interval":"DAY","intervalNum":1,"limit":200000},
	{"rateLimitType":"RAW_REQUESTS","interval":"MINUTE","intervalNum":5,"limit":61000}
	],
	"exchangeFilters":[],
	"symbols":
	[{"symbol":"BTCUSDT","status":"TRADING","baseAsset":"BTC","baseAssetPrecision":8,"quoteAsset":"USDT","quotePrecision":8,"quoteAssetPrecision":8,"baseCommissionPrecision":8,"quoteCommissionPrecision":8,
	"orderTypes":["LIMIT","LIMIT_MAKER","MARKET","STOP_LOSS_LIMIT","TAKE_PROFIT_LIMIT"],
	"icebergAllowed":true,"ocoAllowed":true,"quoteOrderQtyMarketAllowed":true,"allowTrailingStop":true,"cancelReplaceAllowed":true,"isSpotTradingAllowed":true,"isMarginTradingAllowed":true,
	"filters":[
	{"filterType":"PRICE_FILTER","minPrice":"0.01000000","maxPrice":"1000000.00000000","tickSize":"0.01000000"},
	{"filterType":"LOT_SIZE","minQty":"0.00001000","maxQty":"9000.00000000","stepSize":"0.00001000"},
	{"filterType":"ICEBERG_PARTS","limit":10},
	{"filterType":"MARKET_LOT_SIZE","minQty":"0.00000000","maxQty":"97.92836420","stepSize":"0.00000000"},
	{"filterType":"TRAILING_DELTA","minTrailingAboveDelta":10,"maxTrailingAboveDelta":2000,"minTrailingBelowDelta":10,"maxTrailingBelowDelta":2000},
	{"filterType":"PERCENT_PRICE_BY_SIDE","bidMultiplierUp":"5","bidMultiplierDown":"0.2","askMultiplierUp":"5","askMultiplierDown":"0.2","avgPriceMins":5},
	{"filterType":"NOTIONAL","minNotional":"5.00000000","applyMinToMarket":true,"maxNotional":"9000000.00000000","applyMaxToMarket":false,"avgPriceMins":5},
	{"filterType":"MAX_NUM_ORDERS","maxNumOrders":200},{"filterType":"MAX_NUM_ALGO_ORDERS","maxNumAlgoOrders":5}],
	"permissions":["SPOT","MARGIN","TRD_GRP_004","TRD_GRP_005","TRD_GRP_006","TRD_GRP_009","TRD_GRP_010","TRD_GRP_011","TRD_GRP_012","TRD_GRP_013","TRD_GRP_014","TRD_GRP_015","TRD_GRP_016","TRD_GRP_017","TRD_GRP_018","TRD_GRP_019","TRD_GRP_020","TRD_GRP_021","TRD_GRP_022","TRD_GRP_023","TRD_GRP_024","TRD_GRP_025"],
	"defaultSelfTradePreventionMode":"EXPIRE_MAKER",
	"allowedSelfTradePreventionModes":["EXPIRE_TAKER","EXPIRE_MAKER","EXPIRE_BOTH"]}]}
	*/

	string timezone = jsonResponse["timezone"];
	long long stime = jsonResponse["serverTime"];
	string servertime = getDate(stime);

	printf("	timezone : %s\n", timezone.c_str());
	printf("	serverTime : %s\n", servertime.c_str());

	json reatelimits = jsonResponse["rateLimits"];
	if (reatelimits.is_array())
	{
		printf("	rateLimits : \n");

		for (json::iterator it = reatelimits.begin(); it != reatelimits.end(); ++it)
		{
			json ratelimit = *it;

			string rateLimitType = ratelimit["rateLimitType"];
			string interval = ratelimit["interval"];
			long long intervalNum = ratelimit["intervalNum"];
			string intervalNumStr = std::to_string(intervalNum);
			long long limit = ratelimit["limit"];
			string limitStr = std::to_string(limit);

			printf("		{rateLimitType : %s, interval : %s, intervalNum : %s, limit : %s}\n",
				rateLimitType.c_str(),interval.c_str(),intervalNumStr.c_str(),limitStr.c_str());

		}
	}

	json exchangefilters = jsonResponse["exchangeFilters"];
	if (exchangefilters.is_array())
	{
		printf("	exchangeFilters : \n");

		for (json::iterator it = exchangefilters.begin(); it != exchangefilters.end(); ++it)
		{
			json exchangefilter = *it;
		}
	}

	json symbols = jsonResponse["symbols"];
	if (symbols.is_array())
	{
		printf("	symbols : \n");

		for (json::iterator it = symbols.begin(); it != symbols.end(); ++it)
		{
			json symbol = *it;
			
			string sym = symbol["symbol"];
			string status = symbol["status"];
			string baseAsset = symbol["baseAsset"];
			long long baseAssetPrecision = symbol["baseAssetPrecision"];
			string baseAssetPrecisionStr = std::to_string(baseAssetPrecision);
			string quoteAsset = symbol["quoteAsset"];
			long long quotePrecision = symbol["quotePrecision"];
			string quotePrecisionStr = std::to_string(quotePrecision);
			long long quoteAssetPrecision = symbol["quoteAssetPrecision"];
			string quoteAssetPrecisionStr = std::to_string(quoteAssetPrecision);

			printf("		symbol : %s\n", sym.c_str());
			printf("		status : %s\n", status.c_str());
			printf("		baseAsset : %s\n", baseAsset.c_str());
			printf("		baseAssetPrecision : %s\n", baseAssetPrecisionStr.c_str());
			printf("		quoteAsset : %s\n", quoteAsset.c_str());
			printf("		quoteAssetPrecision : %s\n", quoteAssetPrecisionStr.c_str());

			json orderTypes = symbol["orderTypes"];
			json filters = symbol["filters"];
			json permissions = symbol["permissions"];

		}
	}

	printf("----------------------------------------------------------------------------------\n\n");
}


void get_market_order_book(string symbol, int limit = 5)
{
	string strlimit;

	switch (limit)
	{
	case 5:
		strlimit = "5";
		break;
	case 25:
		strlimit = "25";
		break;
	case 50:
		strlimit = "50";
		break;
	case 250:
		strlimit = "250";
		break;
	default:
		break;
	}

	string url = "https://api.binance.com/api/v3/depth?symbol=" + symbol + "&limit=" + strlimit;
	string response = GET(url);

	printf("------------------------------get_market_order_book-------------------------------\n\n");

	auto jsonResponse = json::parse(response);

	long long lastupdateid = jsonResponse["lastUpdateId"];
	string lastUp = std::to_string(lastupdateid);

	json bids = jsonResponse["bids"];
	json asks = jsonResponse["asks"];

	printf("	Last updated ID : %s\n", lastUp.c_str());


	if (bids.is_array()) 
	{
		printf("	bids : \n");

		for (json::iterator it = bids.begin(); it != bids.end(); ++it)
		{
			json bid = *it;
			string price = bid[0];
			string qty = bid[1];

			printf("		{Price : %s, Quantity : %s}\n", price.c_str(), qty.c_str());
		}
	}

	if (asks.is_array())
	{
		printf("	asks : \n");

		for (json::iterator it = asks.begin(); it != asks.end(); ++it)
		{
			json ask = *it;

			string price = ask[0];
			string qty = ask[1];

			printf("		{Price : %s, Quantity : %s}\n", price.c_str(), qty.c_str());
		}
	}

	printf("----------------------------------------------------------------------------------\n\n");

}


void get_market_recent_trades_list(string symbol)
{
	string url = "https://api.binance.com/api/v3/trades?symbol=" + symbol + "&limit=10";
	string response = GET(url);

	auto jsonResponse = json::parse(response);

	printf("-------------------------get_market_recent_trades_list----------------------------\n\n");

	if (jsonResponse.is_array()) {
		for (json::iterator it= jsonResponse.begin(); it != jsonResponse.end(); ++it)
		{
			json trade = *it;
			
			long long tradeID = trade["id"];
			string tradeid = std::to_string(tradeID);

			string price = trade["price"];

			string quantity = trade["qty"];

			long long time = trade["time"];
			string strtime = getDate(time);

			bool isbuyermaker = trade["isBuyerMaker"];
			string isbuyer = isbuyermaker == 1 ? "true" : "false";

			printf("	{Trade ID : %s, Price : %s, Quantity : %s, Time : %s, Buyer is the maker : %s}\n", 
				tradeid.c_str(), price.c_str(), quantity.c_str(), strtime.c_str(), isbuyer.c_str());
		}
	}

	printf("----------------------------------------------------------------------------------\n\n");
}


void get_older_trade_lookup(string symbol)
{
	string url = "https://api.binance.com/api/v3/historicalTrades?symbol=" + symbol;
	string response = GET(url);
	
	//[{"id":3509840759,"price":"64914.13000000","qty":"0.00030000","quoteQty":"19.47423900","time":1711274742529,"isBuyerMaker":false,"isBestMatch":true}]
	
	auto jsonResponse = json::parse(response);

	printf("-------------------------------get_old_trade_price--------------------------------\n\n");

	if (jsonResponse.is_array()) {
		for (json::iterator it = jsonResponse.begin(); it != jsonResponse.end(); ++it)
		{
			json historicaltrade = *it;

			long long id = historicaltrade["id"];
			string tradeid = std::to_string(id);

			string price = historicaltrade["price"];

			string qty = historicaltrade["qty"];

			string quoteQty = historicaltrade["quoteQty"];

			long long time = historicaltrade["time"];
			string strtime = getDate(time);

			bool isBuyerMaker = historicaltrade["isBuyerMaker"];
			string isbuyer = isBuyerMaker == 1 ? "true" : "false";

			bool isBestMatch = historicaltrade["isBestMatch"];
			string isbest = isBestMatch == 1 ? "true" : "false";

			printf("	{id : %s, price : %s, qty : %s, quoteQty : %s, time : %s, isBuyerMaker : %s, isBestMatch : %s}\n",
				tradeid.c_str(), price.c_str(), qty.c_str(), quoteQty.c_str(), strtime.c_str(), isbuyer.c_str(), isbest.c_str());

		}
	}

	printf("----------------------------------------------------------------------------------\n\n");
}


void get_market_current_average_price(string symbol)
{
	string url = "https://api.binance.com/api/v3/avgPrice?symbol=" + symbol;
	string response = GET(url);

	auto jsonResponse = json::parse(response);

	string average_price = jsonResponse["price"];
	long long closetime = jsonResponse["closeTime"];
	string time = getDate(closetime);

	printf("-------------------------get_market_current_average_price-------------------------\n\n");

	printf("	Average price for %s : %s, Last trade time : %s\n", symbol.c_str(), average_price.c_str(),time.c_str());

	printf("----------------------------------------------------------------------------------\n\n");
}


void get_trading_day_ticker(string symbol)
{
	string url = "https://api.binance.com/api/v3/ticker/tradingDay?symbol=" + symbol;
	string response = GET(url);

	auto jsonResponse = json::parse(response);

	/*
	{
  "symbol":             "BTCUSDT",
  "priceChange":        "-83.13000000",         // Absolute price change
  "priceChangePercent": "-0.317",               // Relative price change in percent
  "weightedAvgPrice":   "26234.58803036",       // quoteVolume / volume
  "openPrice":          "26304.80000000",
  "highPrice":          "26397.46000000",
  "lowPrice":           "26088.34000000",
  "lastPrice":          "26221.67000000",
  "volume":             "18495.35066000",       // Volume in base asset
  "quoteVolume":        "485217905.04210480",   // Volume in quote asset
  "openTime":           1695686400000,
  "closeTime":          1695772799999,
  "firstId":            3220151555,             // Trade ID of the first trade in the interval
  "lastId":             3220849281,             // Trade ID of the last trade in the interval
  "count":              697727                  // Number of trades in the interval
	}
	*/

	string sym = jsonResponse["symbol"];
	string priceChange = jsonResponse["priceChange"];
	string priceChangePercent = jsonResponse["priceChangePercent"];
	string weightedAvgPrice = jsonResponse["weightedAvgPrice"];
	string openPrice = jsonResponse["openPrice"];
	string highPrice = jsonResponse["highPrice"];
	string lowPrice = jsonResponse["lowPrice"];
	string lastPrice = jsonResponse["lastPrice"];
	string volume = jsonResponse["volume"];
	string quoteVolume = jsonResponse["quoteVolume"];

	long long openTime = jsonResponse["openTime"];
	string openTimeStr = getDate(openTime);
	long long closeTime = jsonResponse["closeTime"];
	string closeTimeStr = getDate(closeTime);
	long long firstId = jsonResponse["firstId"];
	string firstIdStr = std::to_string(firstId);
	long long lastId = jsonResponse["lastId"];
	string lastIdStr = std::to_string(lastId);
	long long count = jsonResponse["count"];
	string countStr = std::to_string(count);

	printf("-----------------------------get_trading_day_ticker-------------------------------\n\n");

	printf("	symbol : %s\n", sym.c_str());
	printf("	priceChange : %s\n", priceChange.c_str());
	printf("	priceChangePercent : %s\n", priceChangePercent.c_str());
	printf("	weightedAvgPrice : %s\n", weightedAvgPrice.c_str());
	printf("	openPrice : %s\n", openPrice.c_str());
	printf("	highPrice : %s\n", highPrice.c_str());
	printf("	lowPrice : %s\n", lowPrice.c_str());
	printf("	lastPrice : %s\n", lastPrice.c_str());
	printf("	volume : %s\n", volume.c_str());
	printf("	quoteVolume : %s\n", quoteVolume.c_str());

	printf("	openTime : %s\n", openTimeStr.c_str());
	printf("	closeTime : %s\n", closeTimeStr.c_str());
	printf("	firstId : %s\n", firstIdStr.c_str());
	printf("	lastId : %s\n", lastIdStr.c_str());
	printf("	count : %s\n", countStr.c_str());

	printf("----------------------------------------------------------------------------------\n\n");
}


void get_symbol_price_ticker(string symbol)
{
	string url = "https://api.binance.com/api/v3/ticker/price?symbol=" + symbol;
	string response = GET(url);

	auto jsonResponse = json::parse(response);

	string latest_price = jsonResponse["price"];

	printf("-----------------------------get_symbol_price_ticker------------------------------\n\n");

	printf("	Lastest price for %s : %s\n", symbol.c_str(), latest_price.c_str());

	printf("----------------------------------------------------------------------------------\n\n");
}


void get_symbol_order_book_ticker(string symbol)
{
	string url = "https://api.binance.com/api/v3/ticker/bookTicker?symbol=" + symbol;
	string response = GET(url);

	auto jsonResponse = json::parse(response);

	string sym = jsonResponse["symbol"];
	string bidPrice = jsonResponse["bidPrice"];
	string bidQty = jsonResponse["bidQty"];
	string askPrice = jsonResponse["askPrice"];
	string askQty = jsonResponse["askQty"];

	printf("---------------------------get_symbol_order_book_ticker---------------------------\n\n");

	printf("	symbol : %s, bidPrice : %s, bidQty : %s, askPrice : %s, askQty : %s\n",
		sym.c_str(), bidPrice.c_str(), bidQty.c_str(), askPrice.c_str(), askQty.c_str());

	printf("----------------------------------------------------------------------------------\n\n");
}


void get_min_max_price(string symbol)
{
	string url = "https://api.binance.com/api/v3/ticker/24hr?symbol=" + symbol;
	string response = GET(url);

	auto jsonResponse = json::parse(response);

	string lowprice = jsonResponse["lowPrice"];
	string highprice = jsonResponse["highPrice"];

	printf("---------------------------------get_min_max_price--------------------------------\n\n");

	printf("	Min price : %s, Max price : %s\n", lowprice.c_str(), highprice.c_str());

	printf("----------------------------------------------------------------------------------\n\n");
}


void post_order(string api_key, json params)
{
	string url = "https://api.binance.com/api/v3/order?X-MBX-APIKEY=" + api_key;
	string query_string="";

	if (params.is_array()) {
		for (json::iterator it = params.begin(); it != params.end(); ++it) {
			for (json::iterator element = it->begin(); element != it->end(); ++element)
			{
				query_string += element.key() + "=" + (string)element.value();
				if (element+1!=it->end())
					query_string += "&";
			}
		}
	}

	string signature = calculateHMAC(api_secret, query_string);

	url += query_string + "&signature=" + signature;

	string response = GET(url);

	printf("-----------------------------------post_order------------------------------------\n\n");

	printf(response.c_str() + '\n');

	printf("----------------------------------------------------------------------------------\n\n");
}


json bring_balances(string api_key)
{
	string url = "https://api.binance.com/api/v3/account?X-MBX-APIKEY=" + api_key;

	time_t t;
	_time64(&t);

	string stime = std::to_string(t * 1000);

	string signature = calculateHMAC(api_secret, "");

	url += "&timestamp=" + stime + "&signature=" + signature;

	string response = GET(url);

	auto jsonResponse = json::parse(response);

	json balances = {};

	for (const auto& output : jsonResponse["balances"])
	{
		string asset = output["asset"];
		float free = output["free"];
		float locked = output["locked"];
		float total = free + locked;

		if (total > 0)
		{
			balances[asset]["free"].push_back(free);
			balances[asset]["locked"].push_back(locked);
		}
	}

	return balances;
}


void get_position(string symbol, string api_key) {

	string url = "https://api.binance.com/api/v3/account?X-MBX-APIKEY=" + api_key;
	string response = GET(url);

	auto jsonResponse = json::parse(response);

	auto positions = jsonResponse["balances"];

	printf("-----------------------------------get_position----------------------------------\n\n");

	for (const auto& output : jsonResponse["balances"])
	{
		if (output["asset"] == symbol)
		{
			int free = output["free"];
			printf("	Position of %s : %d\n", symbol.c_str(), free);
			return;
		}
	}

	printf("----------------------------------------------------------------------------------\n\n");
}


void connect_binance(string symbol)
{
	if (check_binance_network() == false)
	{
		printf("	Binance connection failed!\n\n");
		return;
	}
	else
	{
		printf("	Binance connection success!\n\n");
	}

	//string symbol = "BTCUSDT";

	//symbol = "ETHBTC";

	get_server_time();

	get_available_symbols();

	get_market_symbol(symbol);

	get_market_exchange_information(symbol);

	get_market_order_book(symbol);

	get_market_recent_trades_list(symbol);

	get_older_trade_lookup(symbol);

	get_market_current_average_price(symbol);

	get_min_max_price(symbol);

	get_trading_day_ticker(symbol);

	get_symbol_price_ticker(symbol);

	get_symbol_order_book_ticker(symbol);

	/***************************************************************************/

	//bring_balances("your_api_key");

	//get_position(symbol, api_key);

	json params;

	params["symbol"] = symbol;
	params["side"] = "BUY";
	params["quantity"] = 1;
	params["price"] = 5000;

	//post_order("your_api_key", params);


}
