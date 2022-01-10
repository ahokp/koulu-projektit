#include "datasourcewidget.hh"

#include <QBoxLayout>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QFrame>
#include <QLabel>
#include <QPushButton>

DataSourceWidget::DataSourceWidget(const DataSourceDetails& sourceDetails, QWidget *parent) : QWidget(parent), sourceDetails(sourceDetails)
{
    // Assemble data source widget box
    this->setStyleSheet("background-color: rgb(109,119,139);");

    topLevelLayout->setSizeConstraint(QLayout::SizeConstraint::SetFixedSize);
    topLevelLayout->addLayout(topBarLayout);

    createTitle();
    createRemoveButton();
    createDivider();
    topLevelLayout->addLayout(controlsLayout);
    createScaleMaximum();
    createScaleMinimum();
    createHide();
}

void DataSourceWidget::createTitle()
{
    QLabel* sourceNameLabel = new QLabel(
                QString::fromStdString(sourceDetails.dataSourceName), this);
    QLabel* dataNameLabel = new QLabel(
                QString::fromStdString(sourceDetails.graphName), this);

    topBarLayout->addWidget(sourceNameLabel);
    topLevelLayout->addWidget(dataNameLabel);
}

void DataSourceWidget::createRemoveButton()
{
    QPushButton* removeSourceButton = new QPushButton("X", this);
    removeSourceButton->setFixedSize(20, 20);
    removeSourceButton->setStyleSheet("background-color: rgb(185, 208, 222)");

    topBarLayout->addWidget(removeSourceButton);

    connect(removeSourceButton, &QPushButton::released,
            [this]()
    {
        emit removed();
    });

}

void DataSourceWidget::createDivider()
{
    QFrame* line1 = new QFrame(this);
    line1->setFrameShape(QFrame::HLine);

    topLevelLayout->addWidget(line1);
}

void DataSourceWidget::createScaleMaximum()
{
    QDoubleSpinBox* graphScaleMaximumSpinBox = new QDoubleSpinBox(this);
    QLabel* graphScaleMaximumLabel = new QLabel(
                QString::fromStdString("Max Y Multiplier"), this);

    graphScaleMaximumSpinBox->setStyleSheet("background-color: rgb(255, 255, 255)");
    graphScaleMaximumSpinBox->setValue(1.00);
    graphScaleMaximumSpinBox->setMinimum(-99.99);
    graphScaleMaximumSpinBox->setSingleStep(0.01);

    controlsLayout->addWidget(graphScaleMaximumLabel, 0, 0);
    controlsLayout->addWidget(graphScaleMaximumSpinBox, 0, 1);

    connect(graphScaleMaximumSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            [this](double d)
    {
        emit editedScaleMaximum(d);
    });
}

void DataSourceWidget::createScaleMinimum()
{
    QLabel* graphScaleMinimumLabel = new QLabel(
                QString::fromStdString("Min Y Multiplier"), this);
    QDoubleSpinBox* graphScaleMinimumSpinBox = new QDoubleSpinBox(this);
    graphScaleMinimumSpinBox->setStyleSheet("background-color: rgb(255, 255, 255)");

    graphScaleMinimumSpinBox->setValue(1.00);
    graphScaleMinimumSpinBox->setMinimum(-99.99);
    graphScaleMinimumSpinBox->setSingleStep(0.01);

    controlsLayout->addWidget(graphScaleMinimumLabel, 1, 0);
    controlsLayout->addWidget(graphScaleMinimumSpinBox, 1, 1);

    connect(graphScaleMinimumSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            [this](double d)
    {
        emit editedScaleMinimum(d);
    });
}

void DataSourceWidget::createHide()
{
    QLabel* graphHideLabel = new QLabel(
                QString::fromStdString("Hide"), this);
    QCheckBox* graphHideCheckBox = new QCheckBox(this);

    controlsLayout->addWidget(graphHideLabel, 2, 0);
    controlsLayout->addWidget(graphHideCheckBox, 2, 1);

    connect(graphHideCheckBox, &QCheckBox::stateChanged,
            [this](int state)
    {
        emit changedState((bool)state);
    });

}

void DataSourceWidget::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    QWidget::paintEvent(event);
}
