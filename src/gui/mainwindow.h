#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringList>
#include <QPropertyAnimation>
#include "../core/src/tokenizer/tokenizer.hpp"
#include "../core/src/translator/traslator.hpp" // Исправлено "traslator" на "translator"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void translateText();
    void clearFields();
    void toggleHistoryPanel();

private:
    Ui::MainWindow *ui;
    QStringList translationHistory;
    QPropertyAnimation *panelAnimation;
    bool isPanelVisible;
    Tokenizer tokenizer;
    Translator *translator;

    QString detectLanguage(const QString &text);
    bool validateInput(const QString &text, const QString &language);
    void updatePanelPosition();
};

#endif // MAINWINDOW_H
