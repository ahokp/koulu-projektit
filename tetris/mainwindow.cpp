/*
  * Name: Paul Ahokas
  * Student number: 283302
  * UserID: 1693
  * E-Mail: paul.ahokas@tuni.fi
  *
  * */

#include "mainwindow.hh"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QKeyEvent>
#include <QtAlgorithms>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // We need a graphics scene in which to draw rectangles.
    scene_ = new QGraphicsScene(this);

    // The graphicsView object is placed on the window
    // at the following coordinates, but if you want
    // different placement, you can change their values.
    int left_margin = 100; // x coordinate
    int top_margin = 150; // y coordinate

    // The width of the graphicsView is BORDER_RIGHT added by 2,
    // since the borders take one pixel on each side
    // (1 on the left, and 1 on the right).
    // Similarly, the height of the graphicsView is BORDER_DOWN added by 2.
    ui->graphicsView->setGeometry(left_margin, top_margin,
                                  BORDER_RIGHT + 2, BORDER_DOWN + 2);
    ui->graphicsView->setScene(scene_);

    // The width of the scene_ is BORDER_RIGHT decreased by 1 and
    // the height of it is BORDER_DOWN decreased by 1, because
    // each square of a tetromino is considered to be inside the sceneRect,
    // if its upper left corner is inside the sceneRect.
    scene_->setSceneRect(0, 0, BORDER_RIGHT - 1, BORDER_DOWN - 1);

    // Setting random engine ready for the first real call.
    int seed = time(0); // You can change seed value for testing purposes
    randomEng.seed(seed);
    distr = std::uniform_int_distribution<int>(0, NUMBER_OF_TETROMINOS - 1);
    distr(randomEng); // Wiping out the first random number (which is almost always 0)
    // After the above settings, you can use randomEng to draw the next falling
    // tetromino by calling: distr(randomEng) in a suitable method.


    ui->infoTextBrowser->setText("^^Press 'Start' to play");
    ui->resetPushButton->setDisabled(true);

    // QTimer for tetromino falling speed
    timer_.setSingleShot(false);
    connect(&timer_, &QTimer::timeout, this, &MainWindow::tetromino_fall);
    // QTimer for keeping track of time and showing it
    connect(&clock_, &QTimer::timeout, this, &MainWindow::showTime);

    // Game difficulty is set to normal by default
    ui->pushButtonNormal->setDisabled(true);
    gameSpeed_ = NORMAL;
    cur_difficulty = NORMAL;

    ui->textBrowserDifficulty->setText("Current difficulty is Normal\n"
                                       "Reset game to set a "
                                       "new difficulty");
    ui->textBrowserDifficulty->hide();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::keyPressEvent(QKeyEvent* event)
{
    // Prevents keyPressEvents if game is inactive.
    if(!timer_.isActive()){
        return;
    }

    // Move down
    if(event->key() == Qt::Key_S){
        // Checks if any square is hitting bottom
        for(auto sq : cur_sq_vec_){
            if(!scene_->sceneRect().contains(sq->x(),
                sq->y() + SQUARE_SIDE)){
                return;
            }
        }
        // Moves squares down
        for(auto sq: cur_sq_vec_){
            sq->moveBy(0, SQUARE_SIDE);
        }
        // Checks for collision between squares
        if(isColliding()){
            // Moves squares back if there is collision
            for(auto sq: cur_sq_vec_){
                sq->moveBy(0, -SQUARE_SIDE);
            }
        }
    }

    // Move left
    if(event->key() == Qt::Key_A){
        // Checks if every square can move left
        for(auto sq: cur_sq_vec_){
            if(!scene_->sceneRect().contains(sq->x() -
                SQUARE_SIDE, sq->y())){

                return;
            }
        }
        // Moves squares
        for(auto sq: cur_sq_vec_){
            sq->moveBy(-SQUARE_SIDE, 0);
        }
        // Checks for collision between squares
        if(isColliding()){
            // Moves squares back if there is collision
            for(auto sq: cur_sq_vec_){
                sq->moveBy(SQUARE_SIDE, 0);
            }
        }

    }

    // Move right
    if(event->key() == Qt::Key_D){
        // Checks if every square can move right
        for(auto sq: cur_sq_vec_){

            if(!scene_->sceneRect().contains(sq->x() +
                SQUARE_SIDE, sq->y())){

                return;
            }
        }
        // Moves squares
        for(auto sq: cur_sq_vec_){
            sq->moveBy(SQUARE_SIDE, 0);
        }
        // Checks for collision between squares
        if(isColliding()){
            // Moves squares back if there is collision
            for(auto sq: cur_sq_vec_){
                sq->moveBy(-SQUARE_SIDE, 0);
            }
        }
    }

    // Move as down as possible
    if(event->key() == Qt::Key_E){

        bool isBottom = false;
        while(true){
            // Checks if any square is hitting bottom
            for(auto sq : cur_sq_vec_){

                if(!scene_->sceneRect().contains(sq->x(),
                    sq->y() + SQUARE_SIDE)){

                    isBottom = true;
                    if(!isColliding()){
                        break;
                    }
                }
                if(isColliding()){

                    // Moves squares back if there is collision
                    for(auto sq: cur_sq_vec_){
                        sq->moveBy(0, -SQUARE_SIDE);
                    }
                    isBottom = true;
                    break;
                }
            }
            if(!isBottom){
                // Moves squares down
                for(auto sq: cur_sq_vec_){
                    sq->moveBy(0, SQUARE_SIDE);
                }
            }
            else{
                break;
            }
        }
    }
}

void MainWindow::tetromino_fall()
{
    // Checks if any square is hitting bottom.
    for(auto sq : cur_sq_vec_){
        if(!scene_->sceneRect().contains(sq->x(),
            sq->y() + SQUARE_SIDE)){

            checkGameState();
            // Prevents generation of new tetromino if game
            // should be over
            if(!timer_.isActive()){
                return;
            }
            generate_tetromino();
            return;
        }
    }
    // Moves every square down
    for(auto sq : cur_sq_vec_){
        sq->moveBy(0, SQUARE_SIDE);
    }
    // Checks for collision between squares
    if(isColliding()){
        // Moves squares back if there is collision
        for(auto sq: cur_sq_vec_){
            sq->moveBy(0, -SQUARE_SIDE);
        }
        checkGameState();
        // Prevents generation of new tetromino if game
        // should be over
        if(!timer_.isActive()){
            return;
        }
        generate_tetromino();
    }
}

void MainWindow::generate_tetromino()
{

    QPen pen(Qt::black);
    // Random colour
    QBrush brush(QColor(42*distr(randomEng), 42*distr(randomEng)
                        , 42*distr(randomEng)));

    // Establishing the squares needed to draw a tetromino
    square1 = scene_->addRect(0, 0, SQUARE_SIDE,
                              SQUARE_SIDE, pen, brush);
    square2 = scene_->addRect(0, 0, SQUARE_SIDE,
                              SQUARE_SIDE, pen, brush);
    square3 = scene_->addRect(0, 0, SQUARE_SIDE,
                              SQUARE_SIDE, pen, brush);
    square4 = scene_->addRect(0, 0, SQUARE_SIDE,
                              SQUARE_SIDE, pen, brush);
    // Get random tetromino
    auto tetr_num = distr(randomEng);

    //### Placing squares in the shape of given tetromino ###
    if(tetr_num == HORIZONTAL){

        square1->setPos(SQUARE_SIDE*4, -SQUARE_SIDE*1);
        square2->setPos(SQUARE_SIDE*5, -SQUARE_SIDE*1);
        square3->setPos(SQUARE_SIDE*6, -SQUARE_SIDE*1);
        square4->setPos(SQUARE_SIDE*7, -SQUARE_SIDE*1);
    }
    else if(tetr_num == LEFT_CORNER){

        square1->setPos(SQUARE_SIDE*4, -SQUARE_SIDE*1);
        square2->setPos(SQUARE_SIDE*4, -SQUARE_SIDE*0);
        square3->setPos(SQUARE_SIDE*5, -SQUARE_SIDE*0);
        square4->setPos(SQUARE_SIDE*6, -SQUARE_SIDE*0);
    }
    else if(tetr_num == RIGHT_CORNER){

        square1->setPos(SQUARE_SIDE*6, -SQUARE_SIDE*1);
        square2->setPos(SQUARE_SIDE*4, -SQUARE_SIDE*0);
        square3->setPos(SQUARE_SIDE*5, -SQUARE_SIDE*0);
        square4->setPos(SQUARE_SIDE*6, -SQUARE_SIDE*0);

    }
    else if(tetr_num == SQUARE){

        square1->setPos(SQUARE_SIDE*4, -SQUARE_SIDE*1);
        square2->setPos(SQUARE_SIDE*4, -SQUARE_SIDE*0);
        square3->setPos(SQUARE_SIDE*5, -SQUARE_SIDE*1);
        square4->setPos(SQUARE_SIDE*5, -SQUARE_SIDE*0);
    }
    else if(tetr_num == STEP_UP_RIGHT){

        square1->setPos(SQUARE_SIDE*4, -SQUARE_SIDE*0);
        square2->setPos(SQUARE_SIDE*5, -SQUARE_SIDE*0);
        square3->setPos(SQUARE_SIDE*5, -SQUARE_SIDE*1);
        square4->setPos(SQUARE_SIDE*6, -SQUARE_SIDE*1);
    }
    else if(tetr_num == PYRAMID){

        square1->setPos(SQUARE_SIDE*5, -SQUARE_SIDE*1);
        square2->setPos(SQUARE_SIDE*4, -SQUARE_SIDE*0);
        square3->setPos(SQUARE_SIDE*5, -SQUARE_SIDE*0);
        square4->setPos(SQUARE_SIDE*6, -SQUARE_SIDE*0);
    }
    else if(tetr_num == STEP_UP_LEFT){

        square1->setPos(SQUARE_SIDE*4, -SQUARE_SIDE*1);
        square2->setPos(SQUARE_SIDE*5, -SQUARE_SIDE*1);
        square3->setPos(SQUARE_SIDE*5, -SQUARE_SIDE*0);
        square4->setPos(SQUARE_SIDE*6, -SQUARE_SIDE*0);
    }
    //#################################################
    cur_sq_vec_ = {square1, square2, square3, square4};

    // Score gets update for each tetromino
    score_ = score_ + 10*EASY/cur_difficulty;
    ui->lcdNumberScore->display(score_);
}

bool MainWindow::isColliding()
{
    for(auto sq: cur_sq_vec_){
        if(sq->collidingItems(Qt::ContainsItemBoundingRect).count() > 0){
            return true;
        }
    }
    return false;
}

void MainWindow::checkGameState()
{
    checkCompleteRows();

    for(auto sq: cur_sq_vec_){
        // Checks if tetromino has reached upper border
        if(sq->y() <= BORDER_UP){

            timer_.stop();
            clock_.stop();
            ui->infoTextBrowser->setText("Game Over.\n"
                                         "Press 'Start' to play again.\n"
                                         "Check your score above!\n"
                                         "Your time is shown below!");
            ui->startPushButton->setEnabled(true);
        }
    }
}

void MainWindow::checkCompleteRows()
{
    // Only squares in current tetromino need to be checked
    for(auto cur_sq: cur_sq_vec_){
        // Keeps count of the squares on the same row as 'cur_sq'
        int count = 0;
        auto y_value = cur_sq->y();

        for(auto sq: scene_->items()){
            // Checks if square is on the same row
            if(y_value == sq->y()){
                count++;
            }
        }

        // Checks if row is full
        if(count == COLUMNS){

            // Removes row
            for(auto sq: scene_->items()){
                // Checks if square is on the same row
                if(y_value == sq->y()){
                    scene_->removeItem(sq);
                }
            }
            for(auto sq: scene_->items()){
                // Checks if square is above removed row
                // moves it down if it is
                if(y_value > sq->y()){
                  sq->moveBy(0, SQUARE_SIDE);
                }
            }
        }
    }
}

void MainWindow::showTime()
{
    seconds_++;

    // Every 15 seconds, game speeds up
    if(seconds_ % 15 == 0){
        // Speed is capped depending on difficulty
        if(gameSpeed_ > cur_difficulty / 10){
            // Game speeds up faster with higher difficulty
            gameSpeed_ = gameSpeed_ -
                        gameSpeed_*seconds_ / cur_difficulty;
        }
        timer_.setInterval(gameSpeed_);
    }

    if(seconds_ == 60){
        minutes_++;
        seconds_ = 0;
    }

    ui->lcdNumberSec->display(seconds_);
    ui->lcdNumberMin->display(minutes_);

}

void MainWindow::on_startPushButton_clicked()
{
    seconds_ = 0;
    minutes_ = 0;
    score_ = 0;
    qDeleteAll(scene_->items());

    ui->startPushButton->setDisabled(true);
    ui->resetPushButton->setEnabled(true);
    clock_.start(1000);
    timer_.start(gameSpeed_);
    ui->infoTextBrowser->setText("'A' to move left, 'D' right\n"
                                 "'S' down and 'E' all the way down\n"
                                 "Have fun!");

    generate_tetromino();
    ui->pushButtonEasy->hide();
    ui->pushButtonNormal->hide();
    ui->pushButtonHard->hide();
    ui->textBrowserDifficulty->show();
}



void MainWindow::on_resetPushButton_clicked()
{
    timer_.stop();
    clock_.stop();
    qDeleteAll(scene_->items());

    ui->startPushButton->setEnabled(true);
    ui->resetPushButton->setDisabled(true);
    ui->infoTextBrowser->setText("Game reset\n"
                                 "Press 'Start' to play again.\n"
                                 "Check your score above!");
    ui->pushButtonEasy->show();
    ui->pushButtonNormal->show();
    ui->pushButtonHard->show();
    ui->textBrowserDifficulty->hide();


}

void MainWindow::on_pushButtonHard_clicked()
{
    gameSpeed_ = HARD;
    cur_difficulty = HARD;
    ui->pushButtonHard->setDisabled(true);
    ui->pushButtonNormal->setEnabled(true);
    ui->pushButtonEasy->setEnabled(true);
    ui->textBrowserDifficulty->setText("Current difficulty is Hard\n"
                                       "Reset game to set a "
                                       "new difficulty");
}

void MainWindow::on_pushButtonNormal_clicked()
{
    gameSpeed_ = NORMAL;
    cur_difficulty = NORMAL;
    ui->pushButtonNormal->setDisabled(true);
    ui->pushButtonHard->setEnabled(true);
    ui->pushButtonEasy->setEnabled(true);
    ui->textBrowserDifficulty->setText("Current difficulty is Normal\n"
                                       "Reset game to set "
                                       "a new difficulty");
}

void MainWindow::on_pushButtonEasy_clicked()
{
    gameSpeed_ = EASY;
    cur_difficulty = EASY;
    ui->pushButtonEasy->setDisabled(true);
    ui->pushButtonNormal->setEnabled(true);
    ui->pushButtonHard->setEnabled(true);
    ui->textBrowserDifficulty->setText("Current difficulty is Easy\n"
                                       "Reset game to set "
                                       "a new difficulty");
}
