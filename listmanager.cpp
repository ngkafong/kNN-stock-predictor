#include "listmanager.h"

/*
 *  ALL INDEX STARTS FROM 1
 *  E.G. Find Item 2, index = 2
 *  The functions will do the work
 *
 */


ListManager::ListManager()
{
    // Do nothing
}


ListManager::~ListManager()
{
    // Do nothing
}


bool ListManager::get_status_downloaded()
{
    return status_downloaded;
}


QString ListManager::get_item_code()
{
    return item_code;
}


int ListManager::get_item_days()
{
    return total_days;
}


double ListManager::get_item_price_from_day(int day)
{
    return priceList[day-1];
}


QVector<double> ListManager::get_price_list()
{
    return priceList;
}


void ListManager::add_new_item_list(QString code, int inputDays, QVector<double> inputList)
{
    item_code = code;
    total_days = inputDays;
    priceList = inputList;
    status_downloaded = true;
}


void ListManager::delete_item()
{
    priceList = {};
    status_downloaded = false;

}


void ListManager::set_status_downloaded(bool status)
{
    status_downloaded = status;
}
