/*
 * Creates new window (widget)
 * for picking new graphs to show
 */

#include "addgraphform.h"
#include "ui_addgraphform.h"
#include "mainwindow.h"

AddGraphForm::AddGraphForm(MainWindow* mainWindow, QWidget* parent) :
    QWidget(parent),
    ui_(new Ui::AddGraphForm), mainWindow_(mainWindow)
{
    ui_->setupUi(this);
    QWidget* centralWidget = new QWidget();
    infoBoxLayout_ = new QVBoxLayout(centralWidget);
    ui_->scrollArea->setWidget(centralWidget);

    auto allDataSourceDetails = mainWindow_->getDataConnector()
            ->getAllDataSourceDetails();

    for (DataSourceDetails& sourceDetails : allDataSourceDetails)
    {
        addGraphToList(sourceDetails);
    }
}

AddGraphForm::~AddGraphForm()
{
    delete ui_;
}

void AddGraphForm::addGraphToList(const DataSourceDetails& sourceDetails)
{
    // Assemble data source widget box
    QWidget* new_widget = new QWidget();
    QGridLayout* layout = new QGridLayout(new_widget);

    layout->setSizeConstraint(QLayout::SizeConstraint::SetMaximumSize);

    new_widget->setStyleSheet("background-color: rgb(109,119,139);");

    QLabel* sourceNameLabel = new QLabel(
                QString::fromStdString(sourceDetails.dataSourceName), new_widget);
    QLabel* dataNameLabel = new QLabel(
                QString::fromStdString(sourceDetails.graphName), new_widget);

    QPushButton* addSourceButton = new QPushButton("Add", new_widget);

    addSourceButton->setStyleSheet("background-color: rgb(185, 208, 222)");
    addSourceButton->setSizePolicy(QSizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed));

    // Connect the Add button to adding a new data source in the main window
    connect(addSourceButton, &QPushButton::released,
            [this, sourceDetails]()
    {
        mainWindow_->createDataSourceWidget(sourceDetails);
        close();
    });

    layout->addWidget(sourceNameLabel, 0, 0);
    layout->addWidget(dataNameLabel, 1, 0);
    layout->addWidget(addSourceButton, 1, 1);

    infoBoxLayout_->addWidget(new_widget);
}

void AddGraphForm::on_searchLineEdit_textEdited(const QString &searchParameter)
{
    for (int i = 0; i < infoBoxLayout_->count(); ++i) {

        QWidget* inspectedWidget = infoBoxLayout_->itemAt(i)->widget();
        inspectedWidget->setVisible(false);
        QList<QLabel *> allLabels = inspectedWidget->findChildren<QLabel *>();

        QString sourceLabelString = allLabels[0]->text().toLower();
        QString dataLabelString = allLabels[1]->text().toLower();

        if (sourceLabelString.toStdString().find(searchParameter.toStdString()) != std::string::npos
                || dataLabelString.toStdString().find(searchParameter.toStdString()) != std::string::npos) {

            inspectedWidget->show();
        }
    }
}
