/*
  * Name: Paul Ahokas
  * Student number: 283302
  * UserID: 1693
  * E-Mail: paul.ahokas@tuni.fi
  *
  * */

#ifndef MAINWINDOW_HH
#define MAINWINDOW_HH

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QTimer>
#include <vector>
#include <random>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void keyPressEvent(QKeyEvent* event) override;

private slots:

    // Starts the game
    void on_startPushButton_clicked();

    // Resets the game
    void on_resetPushButton_clicked();

    // Sets difficulty
    void on_pushButtonHard_clicked();
    void on_pushButtonNormal_clicked();
    void on_pushButtonEasy_clicked();

private:
    Ui::MainWindow *ui;

    QGraphicsScene* scene_;

    // Squares of tetromino
    QGraphicsRectItem* square1;
    QGraphicsRectItem* square2;
    QGraphicsRectItem* square3;
    QGraphicsRectItem* square4;

    // Contains current squares
    // Used for handling current squares in a loop
    std::vector <QGraphicsRectItem*> cur_sq_vec_;

    // Timer for falling speed
    QTimer timer_;
    // Timer for keeping track of time
    QTimer clock_;
    // Dictates how fast tetrominos are falling
    int gameSpeed_;

    // Constants for game speeds on different difficulties (ms)
    const int HARD = 500;
    const int NORMAL = 750;
    const int EASY = 1000;

    // Constants describing scene coordinates
    // Copied from moving circle example and modified a bit
    const int BORDER_UP = 0;
    const int BORDER_DOWN = 480; // 260; (in moving circle)
    const int BORDER_LEFT = 0;
    const int BORDER_RIGHT = 240; // 680; (in moving circle)

    // Size of a tetromino component
    const int SQUARE_SIDE = 20;
    // Number of horizontal cells (places for tetromino components)
    const int COLUMNS = BORDER_RIGHT / SQUARE_SIDE;
    // Number of vertical cells (places for tetromino components)
    const int ROWS = BORDER_DOWN / SQUARE_SIDE;


    // Constants for different tetrominos and the number of them
    enum Tetromino_kind {HORIZONTAL,
                         LEFT_CORNER,
                         RIGHT_CORNER,
                         SQUARE,
                         STEP_UP_RIGHT,
                         PYRAMID,
                         STEP_UP_LEFT,
                         NUMBER_OF_TETROMINOS};
    // From the enum values above, only the last one is needed in this template.
    // Recall from enum type that the value of the first enumerated value is 0,
    // the second is 1, and so on.
    // Therefore the value of the last one is 7 at the moment.
    // Remove those tetromino kinds above that you do not implement,
    // whereupon the value of NUMBER_OF_TETROMINOS changes, too.
    // You can also remove all the other values, if you do not need them,
    // but most probably you need a constant value for NUMBER_OF_TETROMINOS.


    // For randomly selecting the next dropping tetromino
    std::default_random_engine randomEng;
    std::uniform_int_distribution<int> distr;

    int seconds_;
    int minutes_;

    // Keeps track of current difficulty
    // Used for scaling game speed for each difficulty
    int cur_difficulty;

    int score_;

    /**
     * @brief generate_tetromino,
     *        Generates a new tetromino, also counts the
     *        score as it's based around the number tetrominos
     */
    void generate_tetromino();

    /**
     * @brief tetromino_fall,
     *        Handles automatic falling for teromino,
     *        also makes sure a new tetromino is generated
     */
    void tetromino_fall();

    /**
     * @brief isColliding,
     *        Checks if any squares are colliding
     *        (inside eachother)
     * @return true if there is colliding,
     *         false otherwise
     */
    bool isColliding();

    /**
     * @brief checkGameState,
     *        Checks if the game should be over
     */
    void checkGameState();

    /**
     * @brief checkCompleteRows,
     *        Checks for completed rows
     */
    void checkCompleteRows();

    /**
     * @brief showTime
     *        Tracks elapsed time, also handles
     *        automatic game speed increasing
     */
    void showTime();

};

#endif // MAINWINDOW_HH
