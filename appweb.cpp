#include "appweb.h"

//AppWeb::AppWeb(const WEnvironment &env) : WApplication(env) {
//    this->setTitle("Hello world!");
//    root()->addWidget(cpp14::make_unique<WText>("Введите ваше имя: "));
//    this->nameEdit = root()->addWidget(cpp14::make_unique<WLineEdit>("Имя"));
//    this->nameEdit->setFocus();
//    auto button = root()->addWidget(cpp14::make_unique<WPushButton>("Скажи привет!"));
//    button->setMargin(5, Side::Left);
//    root()->addWidget(cpp14::make_unique<WBreak>());
//    this->greeting = root()->addWidget(cpp14::make_unique<WText>());
//    button->clicked().connect(this, &AppWeb::greet);
//    this->nameEdit->enterPressed().connect(this, &AppWeb::greet);
//    upload = root()->addWidget(cpp14::make_unique<Wt::WFileUpload>());
//    upload->setFileTextSize(40);
//    // Provide a button
//    WPushButton *uploadButton = root()->addWidget(cpp14::make_unique<Wt::WPushButton>("Send"));
//    // Upload when the button is clicked.
//    uploadButton->clicked().connect(upload, &Wt::WFileUpload::upload);
//    uploadButton->clicked().connect(uploadButton, &Wt::WPushButton::disable);
//    // React to a succesfull upload.
//    upload->uploaded().connect(this, &AppWeb::fileUploaded);
//    WLink link("test.txt");
//    link.setTarget(LinkTarget::Download);
//    a = root()->addWidget(cpp14::make_unique<WAnchor>(link, "Doc"));
//    WPushButton *test = root()->addWidget(cpp14::make_unique<WPushButton>("Лоад"));
//    // Create an anchor that links to a resource
////    a = root()->addWidget(cpp14::make_unique<Wt::WAnchor>(WLink(cpp14::make_unique<WFileResource>("tablesScheme.txt")), "PDF version"));
//}
//AppWeb::~AppWeb(){}
//void AppWeb::greet(){
//    this->greeting->setText("Привет, " + this->nameEdit->text());
//}
//void AppWeb::fileUploaded(){
//    std::cout << upload->uploadedFiles()[0].spoolFileName() << std::endl; //<--
//}
//void AppWeb::createFile(){
//    std::ofstream out("test.txt");
//    out << "Hello there!";
//    out.close();
////    this->setInternalPath("/test.txt"); creating f*cking "?_="
//}

////////////////////////BASE/////////////////////////////
TableBASE::TableBASE(AppWeb *parent) : WContainerWidget(){
    this->parent = parent;
    table = this->addWidget(cpp14::make_unique<WTable>());
    table->setHeaderCount(1);
    table->addStyleClass("table form-inline");
    table->toggleStyleClass("table-bordered", true);
    table->toggleStyleClass("table-hover", true);

    //LINESELECTOR
    this->addWidget(cpp14::make_unique<WText>("Строка:"));
    lineSelector = this->addWidget(cpp14::make_unique<WComboBox>());
    lineSelector->setMargin(0, Side::Bottom);
    lineSelector->changed().connect(this, [=](){
        currentLine = atoi(lineSelector->currentText().toUTF8().c_str());
        std::cout << "ВЫБРАНА СТРОКА: " << currentLine << std::endl;
        //Now we can kill and edit current item
        lineSelected();
    });

    pushAdd = this->addWidget(cpp14::make_unique<WPushButton>("Добавить"));
    pushKill = this->addWidget(cpp14::make_unique<WPushButton>("Удалить"));
    this->addWidget(cpp14::make_unique<WBreak>());
    this->addWidget(cpp14::make_unique<WBreak>());

    pushKill->setEnabled(false);
}
TableBASE::~TableBASE(){}
void TableBASE::lineSelected(){
    lineSelector->setCurrentIndex(lineSelector->findText(to_string(currentLine)));
    pushKill->setEnabled(true);
    setEnableEditor(true);
}
void TableBASE::lineNOTSelected(){
    lineSelector->setCurrentIndex(-1);
    pushKill->setEnabled(false);
    setEnableEditor(false);
}
void TableBASE::updateLineSelector(){
    lineSelector->clear();
    for (int it = 1; it < table->rowCount(); it++){
        lineSelector->addItem(to_string(it));
    }
}
/////////////////////////////////////////////////////////



////////////////////////GPU//////////////////////////////
TableGPU::TableGPU(AppWeb *parent, Catalog *catalog) : TableBASE(parent){
    this->catalog = catalog;
    connectAll();

    defHeaders();
    updateAllCells();
    updateLineSelector();
    defEditor();
    defEditorConnections();

    lineNOTSelected();
}
TableGPU::~TableGPU(){}
void TableGPU::defHeaders(){
    table->elementAt(0, 0)->addNew<Wt::WText>("Код");
    table->elementAt(0, 1)->addNew<Wt::WText>("Название");
    table->elementAt(0, 2)->addNew<Wt::WText>("NUP");
    table->elementAt(0, 3)->addNew<Wt::WText>("Частота");
    table->elementAt(0, 4)->addNew<Wt::WText>("Производитель");
}
void TableGPU::connectAll(){
    pushAdd->clicked().connect(this, [=](){
        if (catalog->addNewGPU()){
            this->table->insertRow(table->rowCount());
            this->updateCellByRow(table->rowCount()-1);
            currentLine = table->rowCount()-1;
            lineSelected();
            updateLineSelector();
        }
    });
    pushKill->clicked().connect(this, [=](){
        int codeGPU = *catalog->getListGPU()->at(currentLine-1)->getCode();
        if (catalog->deleteGPU(codeGPU)){
            this->table->removeRow(currentLine);
            updateLineSelector();
            if (table->rowCount() != 0){
                if (currentLine-1 != -1){
                    currentLine--;
                }
                lineSelected();
            }
            else {
                lineNOTSelected();
            }
        }
    });

}
void TableGPU::updateAllCells(){
    try {
        this->table->clear();
        defHeaders();
        for (int it = 0; it < catalog->getListGPU()->getSize(); it++){
            vector <string> data = catalog->getListGPU()->at(it)->getVarsVector();
            table->elementAt(it+1, 0)->addNew<Wt::WText>(data[0]);
            table->elementAt(it+1, 1)->addNew<Wt::WText>(data[1]);
            table->elementAt(it+1, 2)->addNew<Wt::WText>(data[2]);
            table->elementAt(it+1, 3)->addNew<Wt::WText>(data[3]);
            table->elementAt(it+1, 4)->addNew<Wt::WText>(data[4]);
        }
        updateLineSelector();
    }
    catch (...) {
        std::cout << "<!> Произошло исключение при попытке обновить ячейки таблицы GPU <!>"
                  << std::endl;
        return;
    }
}
void TableGPU::updateCellByRow(int row){
    if (row < table->rowCount()){
        updateAllCells();
        return;
    }
    try {
        vector <string> data = catalog->getListGPU()->at(row-1)->getVarsVector();
        table->elementAt(row, 0)->clear();
        table->elementAt(row, 0)->addNew<Wt::WText>(data[0]);
        table->elementAt(row, 1)->clear();
        table->elementAt(row, 1)->addNew<Wt::WText>(data[1]);
        table->elementAt(row, 2)->clear();
        table->elementAt(row, 2)->addNew<Wt::WText>(data[2]);
        table->elementAt(row, 3)->clear();
        table->elementAt(row, 3)->addNew<Wt::WText>(data[3]);
        table->elementAt(row, 4)->clear();
        table->elementAt(row, 4)->addNew<Wt::WText>(data[4]);
    }
    catch (...) {
        std::cout << "<!> Произошло исключение при попытке обновить ячейку \""
                     +to_string(row)+"\" таблицы GPU <!>" << std::endl;
        return;
    }
}
void TableGPU::defEditor(){
    this->addWidget(cpp14::make_unique<WText>("Код:"));
    codeValue = this->addWidget(cpp14::make_unique<WLineEdit>());

    this->addWidget(cpp14::make_unique<WText>("Название:"));
    nameValue = this->addWidget(cpp14::make_unique<WLineEdit>());

    this->addWidget(cpp14::make_unique<WText>("NUP:"));
    nupValue = this->addWidget(cpp14::make_unique<WLineEdit>());

    this->addWidget(cpp14::make_unique<WText>("Частота:"));
    freqValue = this->addWidget(cpp14::make_unique<WLineEdit>());

    this->addWidget(cpp14::make_unique<WText>("Производитель:"));
    mrerValue = this->addWidget(cpp14::make_unique<WLineEdit>());

    codeValue->setWidth(WLength("6%"));
    nameValue->setWidth(WLength("20%"));
    nupValue->setWidth(WLength("10%"));
    freqValue->setWidth(WLength("10%"));
    mrerValue->setWidth(WLength("20%"));
}
void TableGPU::defEditorConnections(){
    codeValue->enterPressed().connect(this, [=](){
        GPU *tmp = catalog->getListGPU()->at(currentLine-1);
        if (tmp != nullptr){
            if (!catalog->setGPUcode(tmp, std::stoi(codeValue->text().value()))){
                updateEditorContent();
            }
            else {
                parent->updateAllTables();
            }
        }
        else {
            std::cout << " <!> Ошибка: Функция вернула nullptr <!> " << std::endl;
        }
    });

    nameValue->enterPressed().connect(this, [=](){
        GPU *tmp = catalog->getListGPU()->at(currentLine-1);
        if (tmp != nullptr){
            tmp->setName(nameValue->text().toUTF8());
            catalog->checkGCPtrsVars();
            parent->updateAllTables();
        }
        else {
            std::cout << " <!> Ошибка: Функция вернула nullptr <!> " << std::endl;
        }
    });

    nupValue->enterPressed().connect(this, [=](){
        GPU *tmp = catalog->getListGPU()->at(currentLine-1);
        if (tmp != nullptr){
            tmp->setNUP(std::stoi(nupValue->text().value()));
            parent->updateAllTables();
        }
        else {
            std::cout << " <!> Ошибка: Функция вернула nullptr <!> " << std::endl;
        }
    });

    freqValue->enterPressed().connect(this, [=](){
        GPU *tmp = catalog->getListGPU()->at(currentLine-1);
        if (tmp != nullptr){
            tmp->setFreq(std::stoi(freqValue->text().value()));
            parent->updateAllTables();
        }
        else {
            std::cout << " <!> Ошибка: Функция вернула nullptr <!> " << std::endl;
        }
    });

    mrerValue->enterPressed().connect(this, [=](){
        GPU *tmp = catalog->getListGPU()->at(currentLine-1);
        if (tmp != nullptr){
            tmp->setMRER(mrerValue->text().toUTF8());
            parent->updateAllTables();
        }
        else {
            std::cout << " <!> Ошибка: Функция вернула nullptr <!> " << std::endl;
        }
    });
}
bool TableGPU::updateEditorContent(){
    try {
        vector <string> data = catalog->getListGPU()->at(currentLine-1)->getVarsVector();
        codeValue->setText(data[0]);
        nameValue->setText(data[1]);
        nupValue->setText(data[2]);
        freqValue->setText(data[3]);
        mrerValue->setText(data[4]);
        return true;
    }
    catch (...) {
        return false;
    }
}
void TableGPU::setEnableEditor(bool working){
    if (working){
        if (!updateEditorContent()){
            return;
        }
    }
    codeValue->setEnabled(working);
    nameValue->setEnabled(working);
    nupValue->setEnabled(working);
    freqValue->setEnabled(working);
    mrerValue->setEnabled(working);
}
/////////////////////////////////////////////////////////


////////////////////////MMR//////////////////////////////
TableMMR::TableMMR(AppWeb *parent, Catalog *catalog) : TableBASE(parent){
    this->catalog = catalog;
    connectAll();

    defHeaders();
    updateAllCells();
    updateLineSelector();
    defEditor();
    defEditorConnections();

    lineNOTSelected();
}
TableMMR::~TableMMR(){}
void TableMMR::defHeaders(){
    table->elementAt(0, 0)->addNew<Wt::WText>("Код");
    table->elementAt(0, 1)->addNew<Wt::WText>("Объем (гб)");
    table->elementAt(0, 2)->addNew<Wt::WText>("Пропуск. способ");
    table->elementAt(0, 3)->addNew<Wt::WText>("Частота");
    table->elementAt(0, 4)->addNew<Wt::WText>("Тип");
}
void TableMMR::connectAll(){
    pushAdd->clicked().connect(this, [=](){
        if (catalog->addNewMMR()){
            this->table->insertRow(table->rowCount());
            this->updateCellByRow(table->rowCount()-1);
            currentLine = table->rowCount()-1;
            lineSelected();
            updateLineSelector();
        }
    });
    pushKill->clicked().connect(this, [=](){
        int codeMMR = *catalog->getListMMR()->at(currentLine-1)->getCode();
        if (catalog->deleteMMR(codeMMR)){
            this->table->removeRow(currentLine);
            updateLineSelector();
            if (table->rowCount() != 0){
                if (currentLine-1 != -1){
                    currentLine--;
                }
                lineSelected();
            }
            else {
                lineNOTSelected();
            }
        }
    });

}
void TableMMR::updateAllCells(){
    try {
        this->table->clear();
        defHeaders();
        for (int it = 0; it < catalog->getListMMR()->getSize(); it++){
            vector <string> data = catalog->getListMMR()->at(it)->getVarsVector();
            table->elementAt(it+1, 0)->addNew<Wt::WText>(data[0]);
            table->elementAt(it+1, 1)->addNew<Wt::WText>(data[1]);
            table->elementAt(it+1, 2)->addNew<Wt::WText>(data[2]);
            table->elementAt(it+1, 3)->addNew<Wt::WText>(data[3]);
            table->elementAt(it+1, 4)->addNew<Wt::WText>(data[4]);
        }
        updateLineSelector();
    }
    catch (...) {
        std::cout << "<!> Произошло исключение при попытке обновить ячейки таблицы MMR <!>"
                  << std::endl;
        return;
    }
}
void TableMMR::updateCellByRow(int row){
    if (row < table->rowCount()){
        updateAllCells();
        return;
    }
    try {
        vector <string> data = catalog->getListMMR()->at(row-1)->getVarsVector();
        table->elementAt(row, 0)->clear();
        table->elementAt(row, 0)->addNew<Wt::WText>(data[0]);
        table->elementAt(row, 1)->clear();
        table->elementAt(row, 1)->addNew<Wt::WText>(data[1]);
        table->elementAt(row, 2)->clear();
        table->elementAt(row, 2)->addNew<Wt::WText>(data[2]);
        table->elementAt(row, 3)->clear();
        table->elementAt(row, 3)->addNew<Wt::WText>(data[3]);
        table->elementAt(row, 4)->clear();
        table->elementAt(row, 4)->addNew<Wt::WText>(data[4]);
    }
    catch (...) {
        std::cout << "<!> Произошло исключение при попытке обновить ячейку \""
                     +to_string(row)+"\" таблицы MMR <!>" << std::endl;
        return;
    }
}
void TableMMR::defEditor(){
    this->addWidget(cpp14::make_unique<WText>("Код:"));
    codeValue = this->addWidget(cpp14::make_unique<WLineEdit>());

    this->addWidget(cpp14::make_unique<WText>("Объём:"));
    memoryValue = this->addWidget(cpp14::make_unique<WLineEdit>());

    this->addWidget(cpp14::make_unique<WText>("Пропуск. способ.:"));
    bandWValue = this->addWidget(cpp14::make_unique<WLineEdit>());

    this->addWidget(cpp14::make_unique<WText>("Частота:"));
    freqValue = this->addWidget(cpp14::make_unique<WLineEdit>());

    this->addWidget(cpp14::make_unique<WText>("Тип:"));
    typeValue = this->addWidget(cpp14::make_unique<WLineEdit>());

    codeValue->setWidth(WLength("6%"));
    memoryValue->setWidth(WLength("10%"));
    bandWValue->setWidth(WLength("15%"));
    freqValue->setWidth(WLength("15%"));
    typeValue->setWidth(WLength("10%"));
}
void TableMMR::defEditorConnections(){
    codeValue->enterPressed().connect(this, [=](){
        MMR *tmp = catalog->getListMMR()->at(currentLine-1);
        if (tmp != nullptr){
            if (!catalog->setMMRcode(tmp, std::stoi(codeValue->text().value()))){
                updateEditorContent();
            }
            else {
                parent->updateAllTables();
            }
        }
        else {
            std::cout << " <!> Ошибка: Функция вернула nullptr <!> " << std::endl;
        }
    });

    memoryValue->enterPressed().connect(this, [=](){
        MMR *tmp = catalog->getListMMR()->at(currentLine-1);
        if (tmp != nullptr){
            tmp->setMemory(static_cast<unsigned short>(std::stoi(memoryValue->text().value())));
            parent->updateAllTables();
        }
        else {
            std::cout << " <!> Ошибка: Функция вернула nullptr <!> " << std::endl;
        }
    });

    bandWValue->enterPressed().connect(this, [=](){
        MMR *tmp = catalog->getListMMR()->at(currentLine-1);
        if (tmp != nullptr){
            tmp->setBandW(std::stod(bandWValue->text().value()));
            parent->updateAllTables();
        }
        else {
            std::cout << " <!> Ошибка: Функция вернула nullptr <!> " << std::endl;
        }
    });

    freqValue->enterPressed().connect(this, [=](){
        MMR *tmp = catalog->getListMMR()->at(currentLine-1);
        if (tmp != nullptr){
            tmp->setFreq(std::stoi(freqValue->text().value()));
            parent->updateAllTables();
        }
        else {
            std::cout << " <!> Ошибка: Функция вернула nullptr <!> " << std::endl;
        }
    });

    typeValue->enterPressed().connect(this, [=](){
        MMR *tmp = catalog->getListMMR()->at(currentLine-1);
        if (tmp != nullptr){
            tmp->setType(typeValue->text().toUTF8());
            parent->updateAllTables();
        }
        else {
            std::cout << " <!> Ошибка: Функция вернула nullptr <!> " << std::endl;
        }
    });
}
bool TableMMR::updateEditorContent(){
    try {
        vector <string> data = catalog->getListMMR()->at(currentLine-1)->getVarsVector();
        codeValue->setText(data[0]);
        memoryValue->setText(data[1]);
        bandWValue->setText(data[2]);
        freqValue->setText(data[3]);
        typeValue->setText(data[4]);
        return true;
    }
    catch (...) {
        return false;
    }
}
void TableMMR::setEnableEditor(bool working){
    if (working){
        if (!updateEditorContent()){
            return;
        }
    }
    codeValue->setEnabled(working);
    memoryValue->setEnabled(working);
    bandWValue->setEnabled(working);
    freqValue->setEnabled(working);
    typeValue->setEnabled(working);
}
/////////////////////////////////////////////////////////


////////////////////////MRER//////////////////////////////
TableMRER::TableMRER(AppWeb *parent, Catalog *catalog) : TableBASE(parent){
    this->catalog = catalog;
    connectAll();

    defHeaders();
    updateAllCells();
    updateLineSelector();
    defEditor();
    defEditorConnections();

    lineNOTSelected();
}
TableMRER::~TableMRER(){}
void TableMRER::defHeaders(){
    table->elementAt(0, 0)->addNew<Wt::WText>("Код");
    table->elementAt(0, 1)->addNew<Wt::WText>("Название");
    table->elementAt(0, 2)->addNew<Wt::WText>("Год основания");
    table->elementAt(0, 3)->addNew<Wt::WText>("Сайт");
}
void TableMRER::connectAll(){
    pushAdd->clicked().connect(this, [=](){
        if (catalog->addNewMRER()){
            this->table->insertRow(table->rowCount());
            this->updateCellByRow(table->rowCount()-1);
            currentLine = table->rowCount()-1;
            lineSelected();
            updateLineSelector();
        }
    });
    pushKill->clicked().connect(this, [=](){
        int codeMRER = *catalog->getListMRER()->at(currentLine-1)->getCode();
        if (catalog->deleteMRER(codeMRER)){
            this->table->removeRow(currentLine);
            updateLineSelector();
            if (table->rowCount() != 0){
                if (currentLine-1 != -1){
                    currentLine--;
                }
                lineSelected();
            }
            else {
                lineNOTSelected();
            }
        }
    });

}
void TableMRER::updateAllCells(){
    try {
        this->table->clear();
        defHeaders();
        for (int it = 0; it < catalog->getListMRER()->getSize(); it++){
            vector <string> data = catalog->getListMRER()->at(it)->getVarsVector();
            table->elementAt(it+1, 0)->addNew<Wt::WText>(data[0]);
            table->elementAt(it+1, 1)->addNew<Wt::WText>(data[1]);
            table->elementAt(it+1, 2)->addNew<Wt::WText>(data[2]);
            table->elementAt(it+1, 3)->addNew<Wt::WText>(data[3]);
        }
        updateLineSelector();
    }
    catch (...) {
        std::cout << "<!> Произошло исключение при попытке обновить ячейки таблицы MRER <!>"
                  << std::endl;
        return;
    }
}
void TableMRER::updateCellByRow(int row){
    if (row < table->rowCount()){
        updateAllCells();
        return;
    }
    try {
        vector <string> data = catalog->getListMRER()->at(row-1)->getVarsVector();
        table->elementAt(row, 0)->clear();
        table->elementAt(row, 0)->addNew<Wt::WText>(data[0]);
        table->elementAt(row, 1)->clear();
        table->elementAt(row, 1)->addNew<Wt::WText>(data[1]);
        table->elementAt(row, 2)->clear();
        table->elementAt(row, 2)->addNew<Wt::WText>(data[2]);
        table->elementAt(row, 3)->clear();
        table->elementAt(row, 3)->addNew<Wt::WText>(data[3]);
    }
    catch (...) {
        std::cout << "<!> Произошло исключение при попытке обновить ячейку \""
                     +to_string(row)+"\" таблицы MRER <!>" << std::endl;
        return;
    }
}
void TableMRER::defEditor(){
    this->addWidget(cpp14::make_unique<WText>("Код:"));
    codeValue = this->addWidget(cpp14::make_unique<WLineEdit>());

    this->addWidget(cpp14::make_unique<WText>("Название:"));
    nameValue = this->addWidget(cpp14::make_unique<WLineEdit>());

    this->addWidget(cpp14::make_unique<WText>("Год основания:"));
    fYearValue = this->addWidget(cpp14::make_unique<WLineEdit>());

    this->addWidget(cpp14::make_unique<WText>("Сайт:"));
    siteValue = this->addWidget(cpp14::make_unique<WLineEdit>());

    codeValue->setWidth(WLength("6%"));
    nameValue->setWidth(WLength("20%"));
    fYearValue->setWidth(WLength("10%"));
    siteValue->setWidth(WLength("20%"));
}
void TableMRER::defEditorConnections(){
    codeValue->enterPressed().connect(this, [=](){
        MRER *tmp = catalog->getListMRER()->at(currentLine-1);
        if (tmp != nullptr){
            if (!catalog->setMRERcode(tmp, std::stoi(codeValue->text().value()))){
                updateEditorContent();
            }
            else {
                parent->updateAllTables();
            }
        }
        else {
            std::cout << " <!> Ошибка: Функция вернула nullptr <!> " << std::endl;
        }
    });

    nameValue->enterPressed().connect(this, [=](){
        MRER *tmp = catalog->getListMRER()->at(currentLine-1);
        if (tmp != nullptr){
            tmp->setName(nameValue->text().toUTF8());
            catalog->checkGCPtrsVars();
            parent->updateAllTables();
        }
        else {
            std::cout << " <!> Ошибка: Функция вернула nullptr <!> " << std::endl;
        }
    });

    fYearValue->enterPressed().connect(this, [=](){
        MRER *tmp = catalog->getListMRER()->at(currentLine-1);
        if (tmp != nullptr){
            tmp->setFYear(static_cast<unsigned short>(std::stoi(fYearValue->text().value())));
            parent->updateAllTables();
        }
        else {
            std::cout << " <!> Ошибка: Функция вернула nullptr <!> " << std::endl;
        }
    });

    siteValue->enterPressed().connect(this, [=](){
        MRER *tmp = catalog->getListMRER()->at(currentLine-1);
        if (tmp != nullptr){
            tmp->setSite(siteValue->text().toUTF8());
            parent->updateAllTables();
        }
        else {
            std::cout << " <!> Ошибка: Функция вернула nullptr <!> " << std::endl;
        }
    });
}
bool TableMRER::updateEditorContent(){
    try {
        vector <string> data = catalog->getListMRER()->at(currentLine-1)->getVarsVector();
        codeValue->setText(data[0]);
        nameValue->setText(data[1]);
        fYearValue->setText(data[2]);
        siteValue->setText(data[3]);
        return true;
    }
    catch (...) {
        return false;
    }
}
void TableMRER::setEnableEditor(bool working){
    if (working){
        if (!updateEditorContent()){
            return;
        }
    }
    codeValue->setEnabled(working);
    nameValue->setEnabled(working);
    fYearValue->setEnabled(working);
    siteValue->setEnabled(working);
}
/////////////////////////////////////////////////////////


////////////////////////MRER//////////////////////////////
TableGC::TableGC(AppWeb *parent, Catalog *catalog) : TableBASE(parent){
    this->catalog = catalog;
    connectAll();

    defHeaders();
    updateAllCells();
    updateLineSelector();
    defEditor();
    defEditorConnections();

    lineNOTSelected();
}
TableGC::~TableGC(){}
void TableGC::defHeaders(){
    table->elementAt(0, 0)->addNew<Wt::WText>("Код");
    table->elementAt(0, 1)->addNew<Wt::WText>("Название");
    table->elementAt(0, 2)->addNew<Wt::WText>("Код Производ.");
    table->elementAt(0, 3)->addNew<Wt::WText>("Код Граф. проц.");
    table->elementAt(0, 4)->addNew<Wt::WText>("Код Памяти");
}
void TableGC::connectAll(){
    pushAdd->clicked().connect(this, [=](){
        if (catalog->addNewGC()){
            this->table->insertRow(table->rowCount());
            this->updateCellByRow(table->rowCount()-1);
            currentLine = table->rowCount()-1;
            lineSelected();
            updateLineSelector();
        }
    });
    pushKill->clicked().connect(this, [=](){
        int codeGC = *catalog->getListGC()->at(currentLine-1)->getCode();
        if (catalog->deleteGC(codeGC)){
            this->table->removeRow(currentLine);
            updateLineSelector();
            if (table->rowCount() != 0){
                if (currentLine-1 != -1){
                    currentLine--;
                }
                lineSelected();
            }
            else {
                lineNOTSelected();
            }
        }
    });
}
void TableGC::updateAllCells(){
    try {
        this->table->clear();
        defHeaders();
        for (int it = 0; it < catalog->getListGC()->getSize(); it++){
            vector <string> data = catalog->getListGC()->at(it)->getVarsVector();
            table->elementAt(it+1, 0)->addNew<Wt::WText>(data[0]);
            table->elementAt(it+1, 1)->addNew<Wt::WText>(data[1]);
            table->elementAt(it+1, 2)->addNew<Wt::WText>(data[2]);
            table->elementAt(it+1, 3)->addNew<Wt::WText>(data[3]);
            table->elementAt(it+1, 4)->addNew<Wt::WText>(data[4]);
        }
        updateLineSelector();
    }
    catch (...) {
        std::cout << "<!> Произошло исключение при попытке обновить ячейки таблицы GC <!>"
                  << std::endl;
        return;
    }
}
void TableGC::updateCellByRow(int row){
    if (row < table->rowCount()){
        updateAllCells();
        return;
    }
    try {
        vector <string> data = catalog->getListGC()->at(row-1)->getVarsVector();
        table->elementAt(row, 0)->clear();
        table->elementAt(row, 0)->addNew<Wt::WText>(data[0]);
        table->elementAt(row, 1)->clear();
        table->elementAt(row, 1)->addNew<Wt::WText>(data[1]);
        table->elementAt(row, 2)->clear();
        table->elementAt(row, 2)->addNew<Wt::WText>(data[2]);
        table->elementAt(row, 3)->clear();
        table->elementAt(row, 3)->addNew<Wt::WText>(data[3]);
        table->elementAt(row, 4)->clear();
        table->elementAt(row, 4)->addNew<Wt::WText>(data[4]);
    }
    catch (...) {
        std::cout << "<!> Произошло исключение при попытке обновить ячейку \""
                     +to_string(row)+"\" таблицы GC <!>" << std::endl;
        return;
    }
}
void TableGC::defEditor(){
    this->addWidget(cpp14::make_unique<WText>("Код:"));
    codeValue = this->addWidget(cpp14::make_unique<WLineEdit>());

    this->addWidget(cpp14::make_unique<WText>("Название:"));
    nameValue = this->addWidget(cpp14::make_unique<WLineEdit>());

    this->addWidget(cpp14::make_unique<WText>("Код Производ.:"));
    mrerValue = this->addWidget(cpp14::make_unique<WComboBox>());

    this->addWidget(cpp14::make_unique<WText>("Код Граф. проц.:"));
    gpuValue = this->addWidget(cpp14::make_unique<WComboBox>());

    this->addWidget(cpp14::make_unique<WText>("Код памяти:"));
    mmrValue = this->addWidget(cpp14::make_unique<WComboBox>());


    codeValue->setWidth(WLength("6%"));
    nameValue->setWidth(WLength("25%"));
    mrerValue->setWidth(WLength("6%"));
    gpuValue->setWidth(WLength("6%"));
    mmrValue->setWidth(WLength("6%"));
}
void TableGC::defEditorConnections(){
    codeValue->enterPressed().connect(this, [=](){
        GCard *tmp = catalog->getListGC()->at(currentLine-1);
        if (tmp != nullptr){
            if (!catalog->setGCcode(tmp, std::stoi(codeValue->text()))){
                updateEditorContent();
            }
            else {
                parent->updateAllTables();
            }
        }
        else {
            std::cout << " <!> Ошибка: Функция вернула nullptr <!> " << std::endl;
        }
    });

    nameValue->enterPressed().connect(this, [=](){
        GCard *tmp = catalog->getListGC()->at(currentLine-1);
        if (tmp != nullptr){
            tmp->setName(nameValue->text().toUTF8());
            parent->updateAllTables();
        }
        else {
            std::cout << " <!> Ошибка: Функция вернула nullptr <!> " << std::endl;
        }
    });

    mrerValue->changed().connect(this, [=](){
        GCard *tmp = catalog->getListGC()->at(currentLine-1);
        if (tmp != nullptr){
            if (!catalog->setGCcodeMRER(tmp, std::stoi(mrerValue->valueText()))){
                updateEditorContent();
            }
            else {
                parent->updateAllTables();
            }
        }
        else {
            std::cout << " <!> Ошибка: Функция вернула nullptr <!> " << std::endl;
        }
    });

    gpuValue->changed().connect(this, [=](){
        GCard *tmp = catalog->getListGC()->at(currentLine-1);
        if (tmp != nullptr){
            if (!catalog->setGCcodeGPU(tmp, std::stoi(gpuValue->valueText()))){
                updateEditorContent();
            }
            else {
                parent->updateAllTables();
            }
        }
        else {
            std::cout << " <!> Ошибка: Функция вернула nullptr <!> " << std::endl;
        }
    });

    mmrValue->changed().connect(this, [=](){
        GCard *tmp = catalog->getListGC()->at(currentLine-1);
        if (tmp != nullptr){
            if (!catalog->setGCcodeMMR(tmp, std::stoi(mmrValue->valueText()))){
                updateEditorContent();
            }
            else {
                parent->updateAllTables();
            }
        }
        else {
            std::cout << " <!> Ошибка: Функция вернула nullptr <!> " << std::endl;
        }
    });
}
bool TableGC::updateEditorContent(){
    try {
        vector <string> data = catalog->getListGC()->at(currentLine-1)->getVarsVector();
        codeValue->setText(data[0]);

        nameValue->setText(data[5]);

        mrerValue->clear();
        for (MRER *it : *catalog->getListMRER()->getList()){
            mrerValue->addItem(to_string(*it->getCode()));
        }
        mrerValue->setCurrentIndex(mrerValue->findText(data[2]));

        gpuValue->clear();
        for (GPU *it : *catalog->getListGPU()->getList()){
            gpuValue->addItem(to_string(*it->getCode()));
        }
        gpuValue->setCurrentIndex(gpuValue->findText(data[3]));

        mmrValue->clear();
        for (MMR *it : *catalog->getListMMR()->getList()){
            mmrValue->addItem(to_string(*it->getCode()));
        }
        mmrValue->setCurrentIndex(mmrValue->findText(data[4]));

        return true;
    }
    catch (...) {
        return false;
    }
}
void TableGC::setEnableEditor(bool working){
    if (working){
        if (!updateEditorContent()){
            return;
        }
    }
    codeValue->setEnabled(working);
    nameValue->setEnabled(working);
    mrerValue->setEnabled(working);
    gpuValue->setEnabled(working);
    mmrValue->setEnabled(working);
}
/////////////////////////////////////////////////////////



AppWeb::AppWeb(const WEnvironment &env) : WApplication(env) {
    this->setTitle("Каталог видеокарт");
    setTheme(cpp14::make_unique<WBootstrapTheme>());
    styleXML.setForegroundColor(WColor("red"));
    styleDB.setForegroundColor(WColor("red"));

    WVBoxLayout *mainBox = root()->setLayout(cpp14::make_unique<WVBoxLayout>());

    WHBoxLayout *headBox = mainBox->addLayout(cpp14::make_unique<WHBoxLayout>());

    WVBoxLayout *uploadBox = headBox->addLayout(cpp14::make_unique<WVBoxLayout>());
    uploadBox->addWidget(cpp14::make_unique<WText>("Загрузить файл"));
    uploadFile = uploadBox->addWidget(cpp14::make_unique<Wt::WFileUpload>());
    uploadFile->changed().connect(uploadFile, &Wt::WFileUpload::upload);
    uploadFile->uploaded().connect(this, &AppWeb::fileUploaded);


    WLink link("");
    link.setTarget(LinkTarget::Download);

    WVBoxLayout *downloadBoxXML = headBox->addLayout(cpp14::make_unique<WVBoxLayout>());
    pushDownXML = downloadBoxXML->addWidget(cpp14::make_unique<WPushButton>("Подготовка XML"));
    pushDownXML->clicked().connect(this, &AppWeb::defFileXML);
    downloadXML = downloadBoxXML->addWidget(cpp14::make_unique<WAnchor>(link, "Скачать XML"));
    downloadXML->setStyleClass("downloadxml_href");
    downloadXML->setDecorationStyle(styleDB);
    downloadXML->clicked().connect(this, [=]{
        downloadXML->setLink(nullptr);
        styleXML.setForegroundColor(WColor("red"));
        downloadXML->setDecorationStyle(styleDB);
    });

    WVBoxLayout *downloadBoxDB = headBox->addLayout(cpp14::make_unique<WVBoxLayout>());
    pushDownDB = downloadBoxDB->addWidget(cpp14::make_unique<WPushButton>("Подготовка DB"));
    pushDownDB->clicked().connect(this, &AppWeb::defFileDB);
    downloadDB = downloadBoxDB->addWidget(cpp14::make_unique<WAnchor>(link, "Скачать DB"));
    downloadDB->setDecorationStyle(styleXML);
    downloadDB->clicked().connect(this, [=]{
        downloadDB->setLink(nullptr);
        styleDB.setForegroundColor(WColor("red"));
        downloadDB->setDecorationStyle(styleDB);
    });

    this->catalog = new Catalog();

    auto tabs = mainBox->addWidget(cpp14::make_unique<WTabWidget>());
    tableGPU = static_cast<TableGPU*>(tabs->addTab(cpp14::make_unique<TableGPU>(this, catalog), "Граф. процессор")->contents());
    tableMMR = static_cast<TableMMR*>(tabs->addTab(cpp14::make_unique<TableMMR>(this, catalog), "Память")->contents());
    tableMRER = static_cast<TableMRER*>(tabs->addTab(cpp14::make_unique<TableMRER>(this, catalog), "Производитель")->contents());
    tableGC = static_cast<TableGC*>(tabs->addTab(cpp14::make_unique<TableGC>(this, catalog), "Видеокарты")->contents());
}
AppWeb::~AppWeb(){}
void AppWeb::updateAllTables(TableBASE *table){
    if (table != nullptr){
        table->updateAllCells();
        return;
    }
    tableGPU->updateAllCells();
    tableMMR->updateAllCells();
    tableMRER->updateAllCells();
    tableGC->updateAllCells();
}
void AppWeb::fileUploaded(){

    std::cout << "----------------Загружен файл--------------" << std::endl;
    std::cout << uploadFile->uploadedFiles()[0].clientFileName() << std::endl; //<--
    std::cout << uploadFile->uploadedFiles()[0].spoolFileName() << std::endl; //<--
    std::cout << "-------------------------------------------" << std::endl;

    string exten = uploadFile->uploadedFiles()[0].clientFileName();
    size_t pos = exten.rfind(".");
    if (pos != string::npos){
        if (exten.substr(pos) == ".db"){
            catalog->readFromSQL(uploadFile->uploadedFiles()[0].spoolFileName());
            updateAllTables();
            return;
        }
        else {
            if (exten.substr(pos) == ".xml"){
                catalog->readFromXML(uploadFile->uploadedFiles()[0].spoolFileName());
                updateAllTables();
                return;
            }
        }
    }
}
void AppWeb::defFileXML(){
    catalog->writeToXML("data.xml");
    WLink link("data.xml");
    link.setTarget(LinkTarget::Download);
    styleXML.setForegroundColor(WColor("blue"));
    downloadXML->setDecorationStyle(styleXML);
    downloadXML->setLink(link);
}
void AppWeb::defFileDB(){
    catalog->writeToSQL("data.db");
    WLink link("data.db");
    link.setTarget(LinkTarget::Download);
    styleDB.setForegroundColor(WColor("blue"));
    downloadDB->setDecorationStyle(styleDB);
    downloadDB->setLink(link);
}
