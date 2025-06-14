#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QRegularExpression>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    isPanelVisible(false)
{
    ui->setupUi(this);

    // Настройка historyPanel как наложенного виджета
    ui->historyPanel->setParent(ui->centralWidget);
    updatePanelPosition(); // Устанавливаем начальное положение
    ui->historyPanel->move(-ui->historyPanel->width(), 0); // Скрываем за пределами окна

    // Настройка анимации для панели истории
    panelAnimation = new QPropertyAnimation(ui->historyPanel, "pos");
    panelAnimation->setDuration(300);

    // Подключение сигналов
    connect(ui->translateButton, &QPushButton::clicked, this, &MainWindow::translateText);
    connect(ui->clearButton, &QPushButton::clicked, this, &MainWindow::clearFields);
    connect(ui->toggleHistoryButton, &QPushButton::clicked, this, &MainWindow::toggleHistoryPanel);
    connect(ui->inputTextEdit, &QPlainTextEdit::textChanged, this, &MainWindow::onInputTextChanged);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete panelAnimation;
}

void MainWindow::translateText()
{
    QString inputText = ui->inputTextEdit->toPlainText().trimmed();
    QString sourceLang = ui->sourceLangCombo->currentText();
    QString targetLang = ui->targetLangCombo->currentText();

    // Проверка на пустой ввод
    if (inputText.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите текст для перевода");
        return;
    }

    // Автоопределение языка
    if (sourceLang == "Автоопределение") {
        sourceLang = detectLanguage(inputText);
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
    if (!validateInput(inputText, sourceLang)) {
        QMessageBox::warning(this, "Ошибка", "Невалидные символы для выбранного языка");
        return;
    }

    // Очистка предыдущих результатов
    ui->outputTextBrowser->clear();
    // Варианты уже обновлены через textChanged, оставляем их

    // Заглушка для перевода с помощью нейронной сети
    QString neuralTranslation = QString("Перевод от нейронной сети: %1 -> %2").arg(inputText, targetLang);
    ui->outputTextBrowser->setText(neuralTranslation);

    // Заглушка для онлайн-переводчиков
    bool hasInternet = true; // Здесь проверка наличия интернета
    if (hasInternet) {
        ui->variantsList->addItem("Google Translate: (результат)");
        ui->variantsList->addItem("DeepL: (результат)");
        ui->variantsList->addItem("Yandex Translate: (результат)");
    }

    // Добавление в историю переводов
    QString historyEntry = QString("[%1 -> %2] %3: %4")
                             .arg(sourceLang, targetLang, inputText, neuralTranslation);
    translationHistory.prepend(historyEntry);
    ui->historyList->insertItem(0, historyEntry);
}

void MainWindow::clearFields()
{
    ui->inputTextEdit->clear();
    ui->outputTextBrowser->clear();
    ui->variantsList->clear();
    ui->historyList->clear();
    translationHistory.clear();
    ui->sourceLangCombo->setCurrentIndex(0); // Сброс на "Автоопределение"
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

void MainWindow::onInputTextChanged()
{
    QString inputText = ui->inputTextEdit->toPlainText().trimmed();
    QString targetLang = ui->targetLangCombo->currentText();

    // Очистка предыдущих вариантов
    ui->variantsList->clear();

    // Генерация и отображение вариантов перевода
    if (!inputText.isEmpty()) {
        QStringList variants = generateTranslationVariants(inputText, targetLang);
        for (const QString &variant : variants) {
            ui->variantsList->addItem(variant);
        }
    }
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    updatePanelPosition();

    // Если панель скрыта, убедимся, что она остается за пределами окна
    if (!isPanelVisible) {
        ui->historyPanel->move(-ui->historyPanel->width(), 0);
    }
}

void MainWindow::updatePanelPosition()
{
    // Устанавливаем размер и положение historyPanel
    int panelWidth = ui->historyPanel->minimumWidth();
    int panelHeight = ui->centralWidget->height();
    ui->historyPanel->setGeometry(-panelWidth, 0, panelWidth, panelHeight);
}

QStringList MainWindow::generateTranslationVariants(const QString &text, const QString &targetLang)
{
    // Заглушка для демонстрации вариантов перевода
    QStringList variants;
    if (text.isEmpty()) return variants;

    // Примерные варианты для разных языков (можно заменить на реальную логику)
    if (targetLang == "Русский") {
        if (text.toLower() == "hello") {
            variants << "Привет" << "Здравствуйте" << "Хелло" << "Приветствие" << "Здравствуй";
        } else if (text.toLower() == "world") {
            variants << "Мир" << "Свет" << "Земля" << "Вселенная" << "Мир вокруг";
        } else {
            variants << QString("Перевод 1 для %1").arg(text)
                     << QString("Перевод 2 для %1").arg(text)
                     << QString("Перевод 3 для %1").arg(text);
        }
    } else if (targetLang == "Английский") {
        if (text.toLower() == "привет") {
            variants << "Hello" << "Hi" << "Greetings" << "Hey" << "Salutations";
        } else if (text.toLower() == "мир") {
            variants << "World" << "Peace" << "Earth" << "Universe" << "Globe";
        } else {
            variants << QString("Translation 1 for %1").arg(text)
                     << QString("Translation 2 for %1").arg(text)
                     << QString("Translation 3 for %1").arg(text);
        }
    } else if (targetLang == "Французский") {
        if (text.toLower() == "hello") {
            variants << "Bonjour" << "Salut" << "Allô" << "Bonsoir" << "Salutations";
        } else {
            variants << QString("Traduction 1 pour %1").arg(text)
                     << QString("Traduction 2 pour %1").arg(text)
                     << QString("Traduction 3 pour %1").arg(text);
        }
    } else if (targetLang == "Немецкий") {
        if (text.toLower() == "hello") {
            variants << "Hallo" << "Guten Tag" << "Grüß dich" << "Hallo zusammen" << "Servus";
        } else {
            variants << QString("Übersetzung 1 für %1").arg(text)
                     << QString("Übersetzung 2 für %1").arg(text)
                     << QString("Übersetzung 3 für %1").arg(text);
        }
    } else if (targetLang == "Испанский") {
        if (text.toLower() == "hello") {
            variants << "Hola" << "Buenos días" << "Saludos" << "Ola" << "Qué tal";
        } else {
            variants << QString("Traducción 1 para %1").arg(text)
                     << QString("Traducción 2 para %1").arg(text)
                     << QString("Traducción 3 para %1").arg(text);
        }
    }

    // Ограничиваем количество вариантов до 5
    if (variants.size() > 5) {
        variants = variants.mid(0, 5);
    }

    return variants;
}

QString MainWindow::detectLanguage(const QString &text)
{
    QRegularExpression cyrillic("^[\\p{Cyrillic}\\s\\p{Punctuation}\\d]+$");
    QRegularExpression latin("^[a-zA-Z\\s\\p{Punctuation}\\d]+$");

    if (cyrillic.match(text).hasMatch()) return "Русский";
    if (latin.match(text).hasMatch()) return "Английский";
    return "Неизвестный";
}

bool MainWindow::validateInput(const QString &text, const QString &language)
{
    QRegularExpression regex;
    if (language == "Русский") {
        regex.setPattern("^[\\p{Cyrillic}\\s\\p{Punctuation}\\d]+$");
    } else if (language == "Английский") {
        regex.setPattern("^[a-zA-Z\\s\\p{Punctuation}\\d]+$");
    }
    return regex.match(text).hasMatch();
}
