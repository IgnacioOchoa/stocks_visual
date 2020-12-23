#ifndef SINGLESTOCK_H
#define SINGLESTOCK_H
#include <QDateTime>
#include<QVector>


class SingleStock
{
public:
    SingleStock(QString name, QDateTime initTime, QDateTime endTime);
    QString getName() const {return stockName;}
    void setCloseData(QVector<double>& data) {c_data = data;}
    void appendCloseData(double d) {c_data.append(d);}
    void setHighData(QVector<double>& data) {h_data = data;}
    void appendHighData(double d) {h_data.append(d);}
    void setLowData(QVector<double>& data) {l_data = data;}
    void appendLowData(double d) {l_data.append(d);}
    void setOpenData(QVector<double>& data) {o_data = data;}
    void appendOpenData(double d) {o_data.append(d);}
    void setTimestampData(QVector<long long>& data) {t_data = data;}
    void appendTimestampData(long long d) {t_data.append(d);}
    void setVolumeData(QVector<long long>& data) {v_data = data;}
    void appendVolumeData(long long d) {v_data.append(d);}

    const QVector<double>& getCloseData() const {return c_data;}
    const QVector<double>& getHighData() const {return h_data;}
    const QVector<double>& getLowData() const {return l_data;}
    const QVector<double>& getOpenData() const {return o_data;}
    const QVector<long long>& getTimestampData() const {return t_data;}
    const QVector<long long>& getVolumeData() const {return v_data;}

    QDateTime getInitTime() {return initTime;}
    QDateTime getEndTime() {return endTime;}

private:
    QString stockName;
    QVector<double> c_data;
    QVector<double> h_data;
    QVector<double> l_data;
    QVector<double> o_data;
    QVector<long long> t_data;
    QVector<long long> v_data;
    QDateTime initTime;
    QDateTime endTime;

};

#endif // SINGLESTOCK_H
