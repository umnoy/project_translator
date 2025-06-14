#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringList>
#include <QPropertyAnimation>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void translateText();
    void clearFields();
    void toggleHistoryPanel();
    void onInputTextChanged();

private:
    Ui::MainWindow *ui;
    QStringList translationHistory;
    QPropertyAnimation *panelAnimation;
    bool isPanelVisible;

    QString detectLanguage(const QString &text);
    bool validateInput(const QString &text, const QString &language);
    void updatePanelPosition();
    QStringList generateTranslationVariants(const QString &text, const QString &targetLang);
};

#endif // MAINWINDOW_H