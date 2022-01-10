#ifndef DATASOURCEWIDGET_HH
#define DATASOURCEWIDGET_HH

#include <QWidget>
#include "dataconnector.h"

class DataSourceWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DataSourceWidget(const DataSourceDetails& sourceDetails, QWidget *parent = nullptr);

private:
    const DataSourceDetails sourceDetails;
    QVBoxLayout* topLevelLayout = new QVBoxLayout(this);
    QHBoxLayout* topBarLayout = new QHBoxLayout();
    QGridLayout* controlsLayout = new QGridLayout();


    void createTitle();
    void createRemoveButton();
    void createDivider();
    void createScaleMaximum();
    void createScaleMinimum();
    void createHide();

    void paintEvent(QPaintEvent *event) override;

signals:
    void removed();
    void editedScaleMaximum(const double d);
    void editedScaleMinimum(const double d);
    void changedState(bool state);

};

#endif // DATASOURCEWIDGET_HH
