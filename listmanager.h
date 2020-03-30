#ifndef ITEMLIST_H
#define ITEMLIST_H

#include <QWidget>
#include <QVector>

class ListManager
{

    friend class DataMining;

public:

    ListManager();
    ~ListManager();

    // GET Functions
    bool get_status_downloaded();

    QString get_item_code();

    int get_item_days();

    double get_item_price_from_day(int day); // Get item price from that specified day

    QVector<double> get_price_list(); // Get a list of item pricein vector form

    // Storing Functions
    void add_new_item_list(QString code, int inputDays, QVector<double> inputList);
    void delete_item();
    void set_status_downloaded(bool status);

private:

    QVector<double> priceList;
    QString item_code;
    int total_days = 0;
    bool status_downloaded = false;
};

#endif // ITEMLIST_H

/*
 *  Updates Note
 *  24.11.2019  Started Structuring
 *  26.11.2019  Finished Structuring
 *  28.11.2019  Variables changed with high readability (Starting with "item" & "list", to clarify)
 *              Function variables [index, day] application changed -> More convenient to call
 *  10.12.2019  PriceQueue removed, only using listmanager to store 1 set of data
 *
 */
