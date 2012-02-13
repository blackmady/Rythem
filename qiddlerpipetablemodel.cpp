#include "qiddlerpipetablemodel.h"
#include "qiconnectiondata.h"
#include <QVector>
#include <QStringList>
#include <QDebug>

QiddlerPipeTableModel::QiddlerPipeTableModel(QObject *parent) :
    QAbstractTableModel(parent),pipeNumber(0){
}
QiddlerPipeTableModel::~QiddlerPipeTableModel(){
    blockSignals(true);
    removeAllItem();
    qDebug()<<"~QiddlerPipeTableModel";
}

int QiddlerPipeTableModel::rowCount( const QModelIndex & parent ) const{
    return pipesVector.count();
}
int QiddlerPipeTableModel::columnCount(const QModelIndex &parent) const{
    return 9;
}

QString rypipeDataGetDataByColumn(RyPipeData_ptr p, int column){
    switch(column){
        case 0:
            return QString::number(p->number);
        case 1:
            return QString::number(p->socketConnectionId);
        case 2:
            return ((p->responseStatus.isEmpty())?QString("-"):p->responseStatus);
        case 3:
            return p->httpVersion;
        case 4:
            return p->host;
        case 5:
            return p->serverIp;
        case 6:
            return p->path;
        case 7:
            return QString::number(p->responseBodyRawData().size());
        case 8:
            return p->getResponseHeader("Cache-Control");
        default:
            return QString("-");
    }
}

QVariant QiddlerPipeTableModel::data(const QModelIndex &index, int role) const{
    if(role == Qt::DisplayRole || role == Qt::ToolTipRole){
        int row = index.row();
        int column = index.column();
        RyPipeData_ptr p;
        if(pipesVector.count()>row){
            p = pipesVector.at(row);
        }else{
            return tr("unknown..%1").arg(row);
        }
        if(!p){
            return tr("unknown..2");
        }
        return rypipeDataGetDataByColumn(p,column);

    }else{
        return QVariant();
    }
}
QVariant QiddlerPipeTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    //TODO
    QStringList headers;
    headers<<"#"<<"#2"<<"Result"<<"Protocol"<<"Host"<<"ServerIP"<<"URL"<<"Body"<<"Caching";

    if (orientation == Qt::Horizontal) {
        if(section< headers.size() ){
            return headers[section];
        }else{
            return QString("custom");
        }
    }
    return QVariant();
}
Qt::ItemFlags QiddlerPipeTableModel::flags(const QModelIndex &index) const{
    if(!index.isValid()){
        return Qt::ItemIsEnabled;
    }
    return QAbstractTableModel::flags(index);
}

bool QiddlerPipeTableModel::itemLessThan(RyPipeData_ptr a,RyPipeData_ptr b){
    return rypipeDataGetDataByColumn(a,1) <
                rypipeDataGetDataByColumn(b,1);
}

void QiddlerPipeTableModel::sort(int column, Qt::SortOrder order/* = Qt::AscendingOrder*/){
    //qDebug()<<"sort called..";
    _sortingColumn = column;
    //rypipeDataGetDataByColumn(a,_sortingColumn),
    //rypipeDataGetDataByColumn(b,_sortingColumn)
    //qSort(pipesVector.begin(),pipesVector.end(),itemLessThan);
}

RyPipeData_ptr QiddlerPipeTableModel::getItem(int row){
    //qDebug()<<pipesVector.size()<<row;
    //if(pipesVector.size() >= row){
        return pipesVector.at(row);
    //}
    //qDebug()<<row<<" ---";
    //return RyPipeData_ptr(new RyPipeData());
}

void QiddlerPipeTableModel::updateItem(RyPipeData_ptr p){
    int i = pipesMap.keys().indexOf(p->id);
    if(i!=-1){
        /*
        RyPipeData_ptr ori = pipesMap[p->id];
        pipesMap[p->id] = p;
        int j = pipesVector.indexOf(ori);
        if(j!=-1){
            pipesVector.replace(j,p);
        }
        */
        emit dataChanged(index(i,0),index(i,8));//TODO.. magic number 7
        emit connectionUpdated(p);
    }
}

void QiddlerPipeTableModel::addItem(RyPipeData_ptr p){
    //qDebug()<<"addItem...."<<p->getRequestHeader(QByteArray("Host"))<<pipesVector.count();
    RyPipeData_ptr p1 = p;
    ++pipeNumber;
    p1->number=pipeNumber;

    this->beginInsertRows(index(pipeNumber-1, 0),pipeNumber-1,pipeNumber-1);

    //TODO thread safe?
	pipesMap[p1->id] = p1;
	pipesVector.append(p1);

    //QModelIndex index1 = index(pipeNumber-1, 0);
    //QModelIndex index2 = index(pipeNumber-1, 7);

    this->endInsertRows();
    //emit(dataChanged(index1,index2));
	emit connectionAdded(p);
}

void QiddlerPipeTableModel::removeAllItem(){
    //int l = pipesVector.size();
    pipesMap.clear();
    pipesVector.clear();
    qDebug()<<"length="<<pipesVector.count();
    //emit dataChanged(index(0,0),index(l-1,7));
    emit reset();
}
void QiddlerPipeTableModel::removeItems(){

}
