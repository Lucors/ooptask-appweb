#ifndef APPWEB_H
#define APPWEB_H

#include <Wt/WApplication.h>
#include <Wt/WBreak.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WLineEdit.h>
#include <Wt/WPushButton.h>
#include <Wt/WText.h>
#include <Wt/WTabWidget.h>
#include <Wt/WFileUpload.h>
#include <Wt/WAnchor.h>
#include <Wt/WComboBox.h>
#include <Wt/WTable.h>
#include <Wt/WSignal.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WVBoxLayout.h>
#include <Wt/WFileResource.h>
#include <Wt/WMenuItem.h>
#include <Wt/WBootstrapTheme.h>
#include <Wt/WCssDecorationStyle.h>
#include "bones.h"

using namespace Wt;

class AppWeb;

//TableBASE основа каждой таблицы данных
class TableBASE : public WContainerWidget {
protected:
    AppWeb *parent = nullptr;
    Catalog *catalog = nullptr;
    WTable* table;
    WComboBox *lineSelector;
    WPushButton *pushAdd, *pushKill;
    int currentLine;
public:
    TableBASE(AppWeb *);
    virtual ~TableBASE();
    void lineSelected();
    void lineNOTSelected();
    void updateLineSelector();
    virtual void defHeaders() = 0;
    virtual void connectAll() = 0;
    virtual void updateAllCells() = 0;
    virtual void updateCellByRow(int = -1) = 0;
    virtual void defEditor() = 0;
    virtual void defEditorConnections() = 0;
    virtual bool updateEditorContent() = 0;
    virtual void setEnableEditor(bool) = 0;
};

class TableGPU : public TableBASE {
protected:
    WLineEdit *codeValue;
    WLineEdit *nameValue;
    WLineEdit *nupValue;
    WLineEdit *freqValue;
    WLineEdit *mrerValue;
public:
    TableGPU(AppWeb *, Catalog *);
    virtual ~TableGPU();
    void defHeaders();
    void connectAll();
    void updateAllCells();
    void updateCellByRow(int);
    void defEditor();
    void defEditorConnections();
    bool updateEditorContent();
    void setEnableEditor(bool);
};

class TableMMR : public TableBASE {
protected:
    WLineEdit *codeValue;
    WLineEdit *memoryValue;
    WLineEdit *bandWValue;
    WLineEdit *freqValue;
    WLineEdit *typeValue;
public:
    TableMMR(AppWeb *, Catalog *);
    virtual ~TableMMR();
    void defHeaders();
    void connectAll();
    void updateAllCells();
    void updateCellByRow(int);
    void defEditor();
    void defEditorConnections();
    bool updateEditorContent();
    void setEnableEditor(bool);
};

class TableMRER : public TableBASE {
protected:
    WLineEdit *codeValue;
    WLineEdit *nameValue;
    WLineEdit *fYearValue;
    WLineEdit *siteValue;
public:
    TableMRER(AppWeb *, Catalog *);
    virtual ~TableMRER();
    void defHeaders();
    void connectAll();
    void updateAllCells();
    void updateCellByRow(int);
    void defEditor();
    void defEditorConnections();
    bool updateEditorContent();
    void setEnableEditor(bool);
};

class TableGC : public TableBASE {
protected:
    WLineEdit *codeValue;
    WLineEdit *nameValue;
    WComboBox *mrerValue;
    WComboBox *gpuValue;
    WComboBox *mmrValue;
public:
    TableGC(AppWeb *, Catalog *);
    virtual ~TableGC();
    void defHeaders();
    void connectAll();
    void updateAllCells();
    void updateCellByRow(int);
    void defEditor();
    void defEditorConnections();
    bool updateEditorContent();
    void setEnableEditor(bool);
};

class AppWeb : public WApplication{
private:
    Catalog *catalog;
    TableGPU *tableGPU;
    TableMMR *tableMMR;
    TableMRER *tableMRER;
    TableGC *tableGC;

    WFileUpload *uploadFile;
    WAnchor *downloadXML, *downloadDB;
    WPushButton *pushDownXML, *pushDownDB;

    WCssDecorationStyle styleXML, styleDB;
public:
    AppWeb(const WEnvironment&);
    virtual ~AppWeb();
    void updateAllTables(TableBASE* = nullptr);
    void fileUploaded();
    void defFileXML();
    void defFileDB();
};

#endif // APPWEB_H
