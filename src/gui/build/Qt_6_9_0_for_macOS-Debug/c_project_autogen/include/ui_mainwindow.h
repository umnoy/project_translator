/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QWidget *historyPanel;
    QVBoxLayout *historyPanelLayout;
    QLabel *historyLabel;
    QListWidget *historyList;
    QVBoxLayout *contentLayout;
    QHBoxLayout *langLayout;
    QLabel *sourceLabel;
    QComboBox *sourceLangCombo;
    QLabel *targetLabel;
    QComboBox *targetLangCombo;
    QHBoxLayout *textLayout;
    QPlainTextEdit *inputTextEdit;
    QTextBrowser *outputTextBrowser;
    QListWidget *variantsList;
    QHBoxLayout *buttonLayout;
    QPushButton *toggleHistoryButton;
    QPushButton *translateButton;
    QPushButton *clearButton;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(800, 600);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName("centralWidget");
        historyPanel = new QWidget(centralWidget);
        historyPanel->setObjectName("historyPanel");
        historyPanel->setMinimumSize(QSize(250, 0));
        historyPanel->setMaximumSize(QSize(250, 16777215));
        historyPanelLayout = new QVBoxLayout(historyPanel);
        historyPanelLayout->setObjectName("historyPanelLayout");
        historyPanelLayout->setContentsMargins(0, 0, 0, 0);
        historyLabel = new QLabel(historyPanel);
        historyLabel->setObjectName("historyLabel");

        historyPanelLayout->addWidget(historyLabel);

        historyList = new QListWidget(historyPanel);
        historyList->setObjectName("historyList");

        historyPanelLayout->addWidget(historyList);

        contentLayout = new QVBoxLayout(centralWidget);
        contentLayout->setObjectName("contentLayout");
        langLayout = new QHBoxLayout();
        langLayout->setObjectName("langLayout");
        sourceLabel = new QLabel(centralWidget);
        sourceLabel->setObjectName("sourceLabel");

        langLayout->addWidget(sourceLabel);

        sourceLangCombo = new QComboBox(centralWidget);
        sourceLangCombo->addItem(QString());
        sourceLangCombo->addItem(QString());
        sourceLangCombo->addItem(QString());
        sourceLangCombo->addItem(QString());
        sourceLangCombo->addItem(QString());
        sourceLangCombo->addItem(QString());
        sourceLangCombo->setObjectName("sourceLangCombo");

        langLayout->addWidget(sourceLangCombo);

        targetLabel = new QLabel(centralWidget);
        targetLabel->setObjectName("targetLabel");

        langLayout->addWidget(targetLabel);

        targetLangCombo = new QComboBox(centralWidget);
        targetLangCombo->addItem(QString());
        targetLangCombo->addItem(QString());
        targetLangCombo->addItem(QString());
        targetLangCombo->addItem(QString());
        targetLangCombo->addItem(QString());
        targetLangCombo->setObjectName("targetLangCombo");

        langLayout->addWidget(targetLangCombo);


        contentLayout->addLayout(langLayout);

        textLayout = new QHBoxLayout();
        textLayout->setObjectName("textLayout");
        inputTextEdit = new QPlainTextEdit(centralWidget);
        inputTextEdit->setObjectName("inputTextEdit");

        textLayout->addWidget(inputTextEdit);

        outputTextBrowser = new QTextBrowser(centralWidget);
        outputTextBrowser->setObjectName("outputTextBrowser");
        outputTextBrowser->setReadOnly(true);

        textLayout->addWidget(outputTextBrowser);


        contentLayout->addLayout(textLayout);

        variantsList = new QListWidget(centralWidget);
        variantsList->setObjectName("variantsList");

        contentLayout->addWidget(variantsList);

        buttonLayout = new QHBoxLayout();
        buttonLayout->setObjectName("buttonLayout");
        toggleHistoryButton = new QPushButton(centralWidget);
        toggleHistoryButton->setObjectName("toggleHistoryButton");

        buttonLayout->addWidget(toggleHistoryButton);

        translateButton = new QPushButton(centralWidget);
        translateButton->setObjectName("translateButton");

        buttonLayout->addWidget(translateButton);

        clearButton = new QPushButton(centralWidget);
        clearButton->setObjectName("clearButton");

        buttonLayout->addWidget(clearButton);


        contentLayout->addLayout(buttonLayout);

        MainWindow->setCentralWidget(centralWidget);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "Multilingual Translator", nullptr));
        historyLabel->setText(QCoreApplication::translate("MainWindow", "\320\230\321\201\321\202\320\276\321\200\320\270\321\217 \320\277\320\265\321\200\320\265\320\262\320\276\320\264\320\276\320\262:", nullptr));
        sourceLabel->setText(QCoreApplication::translate("MainWindow", "\320\230\321\201\321\205\320\276\320\264\320\275\321\213\320\271 \321\217\320\267\321\213\320\272:", nullptr));
        sourceLangCombo->setItemText(0, QCoreApplication::translate("MainWindow", "\320\220\320\262\321\202\320\276\320\276\320\277\321\200\320\265\320\264\320\265\320\273\320\265\320\275\320\270\320\265", nullptr));
        sourceLangCombo->setItemText(1, QCoreApplication::translate("MainWindow", "\320\240\321\203\321\201\321\201\320\272\320\270\320\271", nullptr));
        sourceLangCombo->setItemText(2, QCoreApplication::translate("MainWindow", "\320\220\320\275\320\263\320\273\320\270\320\271\321\201\320\272\320\270\320\271", nullptr));
        sourceLangCombo->setItemText(3, QCoreApplication::translate("MainWindow", "\320\244\321\200\320\260\320\275\321\206\321\203\320\267\321\201\320\272\320\270\320\271", nullptr));
        sourceLangCombo->setItemText(4, QCoreApplication::translate("MainWindow", "\320\235\320\265\320\274\320\265\321\206\320\272\320\270\320\271", nullptr));
        sourceLangCombo->setItemText(5, QCoreApplication::translate("MainWindow", "\320\230\321\201\320\277\320\260\320\275\321\201\320\272\320\270\320\271", nullptr));

        targetLabel->setText(QCoreApplication::translate("MainWindow", "\320\257\320\267\321\213\320\272 \321\200\320\265\320\267\321\203\320\273\321\214\321\202\320\260\321\202\320\260:", nullptr));
        targetLangCombo->setItemText(0, QCoreApplication::translate("MainWindow", "\320\240\321\203\321\201\321\201\320\272\320\270\320\271", nullptr));
        targetLangCombo->setItemText(1, QCoreApplication::translate("MainWindow", "\320\220\320\275\320\263\320\273\320\270\320\271\321\201\320\272\320\270\320\271", nullptr));
        targetLangCombo->setItemText(2, QCoreApplication::translate("MainWindow", "\320\244\321\200\320\260\320\275\321\206\321\203\320\267\321\201\320\272\320\270\320\271", nullptr));
        targetLangCombo->setItemText(3, QCoreApplication::translate("MainWindow", "\320\235\320\265\320\274\320\265\321\206\320\272\320\270\320\271", nullptr));
        targetLangCombo->setItemText(4, QCoreApplication::translate("MainWindow", "\320\230\321\201\320\277\320\260\320\275\321\201\320\272\320\270\320\271", nullptr));

        inputTextEdit->setPlaceholderText(QCoreApplication::translate("MainWindow", "\320\222\320\262\320\265\320\264\320\270\321\202\320\265 \321\202\320\265\320\272\321\201\321\202 \320\264\320\273\321\217 \320\277\320\265\321\200\320\265\320\262\320\276\320\264\320\260...", nullptr));
        toggleHistoryButton->setText(QCoreApplication::translate("MainWindow", "\320\237\320\276\320\272\320\260\320\267\320\260\321\202\321\214 \320\270\321\201\321\202\320\276\321\200\320\270\321\216", nullptr));
        translateButton->setText(QCoreApplication::translate("MainWindow", "\320\237\320\265\321\200\320\265\320\262\320\265\321\201\321\202\320\270", nullptr));
        clearButton->setText(QCoreApplication::translate("MainWindow", "\320\236\321\207\320\270\321\201\321\202\320\270\321\202\321\214", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
