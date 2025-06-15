#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QRegularExpression>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    isPanelVisible(false),
    tokenizer("/home/kali/Desktop/project_translator/src/core/opus-mt-en-ru/vocab.json")
{
    ui->setupUi(this);

    // Настройка historyPanel как наложенного виджета
    ui->historyPanel->setParent(ui->centralWidget);
    updatePanelPosition();
    ui->historyPanel->move(-ui->historyPanel->width(), 0);

    // Настройка анимации для панели истории
    panelAnimation = new QPropertyAnimation(ui->historyPanel, "pos");
    panelAnimation->setDuration(300);

    // Ограничение выбора языков: Русский, Английский
    ui->sourceLangCombo->clear();
    ui->sourceLangCombo->addItems({"Автоопределение", "Русский", "Английский"});
    ui->targetLangCombo->clear();
    ui->targetLangCombo->addItems({"Русский", "Английский"});

    // Подключение сигналов
    connect(ui->translateButton, &QPushButton::clicked, this, &MainWindow::translateText);
    connect(ui->clearButton, &QPushButton::clicked, this, &MainWindow::clearFields);
    connect(ui->toggleHistoryButton, &QPushButton::clicked, this, &MainWindow::toggleHistoryPanel);

    // Инициализация Translator (по умолчанию en->ru)
    try {
        translator = new Translator(
            tokenizer,
            "/home/kali/Desktop/project_translator/src/core/opus-mt-en-ru/encoder.onnx",
            "/home/kali/Desktop/project_translator/src/core/opus-mt-en-ru/decoder.onnx",
            62517, // pad_token_id
            0,     // eos_token_id
            50,    // max_length
            3      // beam_width
        );
    } catch (const std::exception &e) {
        QMessageBox::critical(this, "Ошибка", QString("Не удалось инициализировать переводчик: %1").arg(e.what()));
        translator = nullptr;
    }
}

MainWindow::~MainWindow()
{
    delete translator;
    delete panelAnimation;
    delete ui;
}

void MainWindow::translateText()
{
    QString inputText = ui->inputTextEdit->toPlainText().trimmed();
    // Удаляем переносы строк и лишние пробелы
    inputText = inputText.replace("\n", " ").simplified();
    QString sourceLang = ui->sourceLangCombo->currentText();
    QString targetLang = ui->targetLangCombo->currentText();

    // Отладочный вывод
    qDebug() << "Input text:" << inputText << "Source lang:" << sourceLang << "Target lang:" << targetLang;

    // Проверка на пустой ввод
    if (inputText.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите текст для перевода");
        return;
    }

    // Автоопределение языка
    if (sourceLang == "Автоопределение") {
        sourceLang = detectLanguage(inputText);
        qDebug() << "Detected language:" << sourceLang;
        int index = ui->sourceLangCombo->findText(sourceLang);
        if (index != -1) {
            ui->sourceLangCombo->setCurrentIndex(index);
        } else {
            QMessageBox::warning(this, "Ошибка", "Не удалось определить язык");
            return;
        }
    }

    // Проверка на одинаковые языки
    if (sourceLang == targetLang) {
        QMessageBox::warning(this, "Ошибка", "Исходный и целевой языки не должны совпадать");
        return;
    }

    // Валидация символов
    /*
    if (!validateInput(inputText, sourceLang)) {
        QMessageBox::warning(this, "Ошибка", QString("Невалидные символы для языка: %1").arg(sourceLang));
        return;
    }
*/
    // Выбор модели в зависимости от языковой пары
    try {
        delete translator;
        translator = nullptr;
        if (sourceLang == "Русский" && targetLang == "Английский") {
            translator = new Translator(
                tokenizer,
                "/home/kali/Desktop/project_translator/src/core/opus-mt-ru-en/encoder.onnx",
                "/home/kali/Desktop/project_translator/src/core/opus-mt-ru-en/decoder.onnx",
                62517, // pad_token_id
                0,     // eos_token_id
                50,    // max_length
                3      // beam_width
            );
        } else if (sourceLang == "Английский" && targetLang == "Русский") {
            translator = new Translator(
                tokenizer,
                "/home/kali/Desktop/project_translator/src/core/opus-mt-en-ru/encoder.onnx",
                "/home/kali/Desktop/project_translator/src/core/opus-mt-en-ru/decoder.onnx",
                62517, // pad_token_id
                0,     // eos_token_id
                50,    // max_length
                3      // beam_width
            );
        }
        if (translator) {
            QString neuralTranslation = QString::fromStdString(translator->run(inputText.toStdString()));
            ui->outputTextBrowser->setText(neuralTranslation);

            // Добавление в историю переводов
            QString historyEntry = QString("[%1 -> %2] %3: %4")
                                    .arg(sourceLang, targetLang, inputText, neuralTranslation);
            translationHistory.prepend(historyEntry);
            ui->historyList->insertItem(0, historyEntry);
        } else {
            QMessageBox::critical(this, "Ошибка", "Переводчик не инициализирован");
        }
    } catch (const std::exception &e) {
        QMessageBox::warning(this, "Ошибка", QString("Ошибка перевода: %1").arg(e.what()));
    }
}

void MainWindow::clearFields()
{
    ui->inputTextEdit->clear();
    ui->outputTextBrowser->clear();
    ui->historyList->clear();
    translationHistory.clear();
    ui->sourceLangCombo->setCurrentIndex(0);
}

void MainWindow::toggleHistoryPanel()
{
    if (isPanelVisible) {
        panelAnimation->setStartValue(QPoint(0, 0));
        panelAnimation->setEndValue(QPoint(-ui->historyPanel->width(), 0));
        ui->toggleHistoryButton->setText("Показать историю");
    } else {
        panelAnimation->setStartValue(QPoint(-ui->historyPanel->width(), 0));
        panelAnimation->setEndValue(QPoint(0, 0));
        ui->toggleHistoryButton->setText("Скрыть историю");
    }
    panelAnimation->start();
    isPanelVisible = !isPanelVisible;
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    updatePanelPosition();

    if (!isPanelVisible) {
        ui->historyPanel->move(-ui->historyPanel->width(), 0);
    }
}

void MainWindow::updatePanelPosition()
{
    int panelWidth = ui->historyPanel->minimumWidth();
    int panelHeight = ui->centralWidget->height();
    ui->historyPanel->setGeometry(-panelWidth, 0, panelWidth, panelHeight);
}

QString MainWindow::detectLanguage(const QString &text)
{
    QRegularExpression cyrillic("^[\\p{Cyrillic}\\s\\p{Punct}\\d]+$");
    QRegularExpression latin("^[a-zA-Z\\s\\p{Punct}\\d\\-\\'\\@]+$");
    cyrillic.setPatternOptions(QRegularExpression::UseUnicodePropertiesOption);
    latin.setPatternOptions(QRegularExpression::UseUnicodePropertiesOption);

    if (!cyrillic.isValid() || !latin.isValid()) {
        qDebug() << "Invalid regex in detectLanguage. Cyrillic:" << cyrillic.pattern()
                 << "Latin:" << latin.pattern();
        return "Неизвестный";
    }

    if (cyrillic.match(text).hasMatch()) return "Русский";
    if (latin.match(text).hasMatch()) return "Английский";
    return "Неизвестный";
}

bool MainWindow::validateInput(const QString &text, const QString &language)
{
    if (language != "Русский" && language != "Английский") {
        qDebug() << "Invalid language in validateInput:" << language;
        return false;
    }

    QRegularExpression regex;
    if (language == "Русский") {
        regex.setPattern("^[\\p{Cyrillic}\\s\\p{Punct}\\d]+$");
    } else if (language == "Английский") {
        regex.setPattern("^[a-zA-Z\\s\\p{Punct}\\d\\-\\'\\@]+$");
    }
    regex.setPatternOptions(QRegularExpression::UseUnicodePropertiesOption);

    if (!regex.isValid()) {
        qDebug() << "Invalid regex pattern for language:" << language
                 << "Pattern:" << regex.pattern();
        return false;
    }

    bool isValid = regex.match(text).hasMatch();
    qDebug() << "Validation for" << language << "text:" << text << "result:" << isValid;
    return isValid;
}
