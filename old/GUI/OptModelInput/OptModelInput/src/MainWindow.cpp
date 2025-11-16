#include "MainWindow.h"

MainWindow::MainWindow(QWidget *parent) : 
    QMainWindow(parent),
    m_previousIndex(),
    m_wasPlayersView(true)
{
    ui.setupUi(this);
    
    m_isPresentWidget = new IsPresentWidget(this);
    ui.isPresentWidget->setLayout(new QVBoxLayout);
    ui.isPresentWidget->layout()->addWidget(m_isPresentWidget);

    connect(ui.actionOpen, SIGNAL(triggered()), this, SLOT(onActionOpen()));
    connect(ui.actionSave_as, SIGNAL(triggered()), this, SLOT(onModelSave()));
    connect(ui.saveButtonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(onSaveButtonBoxClicked(QAbstractButton*)));
    connect(ui.addObjectButton, SIGNAL(clicked(bool)), this, SLOT(onObjectAdd()));
    connect(ui.deleteObjectButton, SIGNAL(clicked(bool)), this, SLOT(onObjectDelete()));
    connectContextUpdateSignals();

    updateUI();
}

MainWindow::~MainWindow()
{}

void MainWindow::connectContextUpdateSignals()
{
    //connect(ui.playersRadioButton, SIGNAL(clicked(bool)), this, SLOT(updateUI()));
    connect(ui.playersRadioButton, SIGNAL(clicked(bool)), this, SLOT(updateContextAndUI()));
    //connect(ui.tasksRadioButton, SIGNAL(clicked(bool)), this, SLOT(updateUI()));
    connect(ui.tasksRadioButton, SIGNAL(clicked(bool)), this, SLOT(updateContextAndUI()));

    m_isPresentWidget->connectContextUpdateSignals(this);
    //connect(ui.saveListWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(updateUI()));
    connect(ui.saveListWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(updateContextAndUI()));

    connect(ui.aSlider, SIGNAL(valueChanged(int)), this, SLOT(updateObjectContext()));
    connect(ui.bSlider, SIGNAL(valueChanged(int)), this, SLOT(updateObjectContext()));
    connect(ui.cSlider, SIGNAL(valueChanged(int)), this, SLOT(updateObjectContext()));
    connect(ui.taskDurationSpinBox, SIGNAL(valueChanged(int)), this, SLOT(updateObjectContext()));
}

void MainWindow::disconnectContextUpdateSignals()
{
    //disconnect(ui.playersRadioButton, SIGNAL(clicked(bool)), this, SLOT(updateUI()));
    disconnect(ui.playersRadioButton, SIGNAL(clicked(bool)), this, SLOT(updateContextAndUI()));
    //disconnect(ui.tasksRadioButton, SIGNAL(clicked(bool)), this, SLOT(updateUI()));
    disconnect(ui.tasksRadioButton, SIGNAL(clicked(bool)), this, SLOT(updateContextAndUI()));
    
    m_isPresentWidget->disconnectContextUpdateSignals(this);
    //disconnect(ui.saveListWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(updateUI()));
    disconnect(ui.saveListWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(updateContextAndUI()));
    
    disconnect(ui.aSlider, SIGNAL(valueChanged(int)), this, SLOT(updateObjectContext()));
    disconnect(ui.bSlider, SIGNAL(valueChanged(int)), this, SLOT(updateObjectContext()));
    disconnect(ui.cSlider, SIGNAL(valueChanged(int)), this, SLOT(updateObjectContext()));
    disconnect(ui.taskDurationSpinBox, SIGNAL(valueChanged(int)), this, SLOT(updateObjectContext()));
}

void MainWindow::onSaveButtonBoxClicked(QAbstractButton* _button)
{
    auto buttons = ui.saveButtonBox->buttons();
    for (auto button : buttons) {
        if (button == _button) {
            switch (ui.saveButtonBox->standardButton(button)) {
                case QDialogButtonBox::Save:
                    onModelSave();
                    break;
                default:
                    break;
            }
            break;
        }
    }
}

void MainWindow::onModelSave()
{
    QString path = QFileDialog::getSaveFileName(
        this,
        "Browse to model data file save location",
        QDir::currentPath(),
        "XML files (*.xml);;Text files (*.txt)");

    if (path.isEmpty())
    {
        qDebug() << "File path is empty!";
        return;
    }

    QDomDocument doc;
    QDomElement root = doc.createElement("model");
    doc.appendChild(root);

    QDomElement playersElem = doc.createElement("players");
    root.appendChild(playersElem);

    for (auto player : m_players) {
        QDomElement playerElem = doc.createElement("player");
        playersElem.appendChild(playerElem);

        QDomElement playerNameElem = doc.createElement("name");
        playerElem.appendChild(playerNameElem);
        QDomText playerNameText = doc.createTextNode(player->m_name);
        playerNameElem.appendChild(playerNameText);

        QDomElement playerNeedsElem = doc.createElement("needs");
        playerElem.appendChild(playerNeedsElem);
        QDomElement aElem = doc.createElement("a");
        QDomText aElemText = doc.createTextNode(QString::number(player->m_a));
        aElem.appendChild(aElemText);
        playerNeedsElem.appendChild(aElem);
        QDomElement bElem = doc.createElement("b");
        QDomText bElemText = doc.createTextNode(QString::number(player->m_b));
        bElem.appendChild(bElemText);
        playerNeedsElem.appendChild(bElem);
        QDomElement cElem = doc.createElement("c");
        QDomText cElemText = doc.createTextNode(QString::number(player->m_c));
        cElem.appendChild(cElemText);
        playerNeedsElem.appendChild(cElem);

        QDomElement scheduleElem = doc.createElement("schedule");
        playerElem.appendChild(scheduleElem);
        QDomElement mondayElem = doc.createElement("monday");
        scheduleElem.appendChild(mondayElem);
        QDomElement mondayStartElem = doc.createElement("start");
        mondayElem.appendChild(mondayStartElem);
        QDomText mondayStartText = doc.createTextNode(player->m_startTime[MONDAY]);
        mondayStartElem.appendChild(mondayStartText);
        QDomElement mondayEndElem = doc.createElement("end");
        mondayElem.appendChild(mondayEndElem);
        QDomText mondayEndText = doc.createTextNode(player->m_endTime[MONDAY]);
        mondayEndElem.appendChild(mondayEndText);

        QDomElement wednesdayElem = doc.createElement("wednesday");
        scheduleElem.appendChild(wednesdayElem);
        QDomElement wednesdayStartElem = doc.createElement("start");
        wednesdayElem.appendChild(wednesdayStartElem);
        QDomText wednesdayStartText = doc.createTextNode(player->m_startTime[WEDNESDAY]);
        wednesdayStartElem.appendChild(wednesdayStartText);
        QDomElement wednesdayEndElem = doc.createElement("end");
        wednesdayElem.appendChild(wednesdayEndElem);
        QDomText wednesdayEndText = doc.createTextNode(player->m_endTime[WEDNESDAY]);
        wednesdayEndElem.appendChild(wednesdayEndText);

        QDomElement fridayElem = doc.createElement("friday");
        scheduleElem.appendChild(fridayElem);
        QDomElement fridayStartElem = doc.createElement("start");
        fridayElem.appendChild(fridayStartElem);
        QDomText fridayStartText = doc.createTextNode(player->m_startTime[FRIDAY]);
        fridayStartElem.appendChild(fridayStartText);
        QDomElement fridayEndElem = doc.createElement("end");
        fridayElem.appendChild(fridayEndElem);
        QDomText fridayEndText = doc.createTextNode(player->m_endTime[FRIDAY]);
        fridayEndElem.appendChild(fridayEndText);
    }

    QDomElement exercisesElem = doc.createElement("exercises");
    root.appendChild(exercisesElem);

    for (auto exercise : m_exercises) {
        QDomElement exerciseElem = doc.createElement("exercise");
        exercisesElem.appendChild(exerciseElem);

        QDomElement exerciseNameElem = doc.createElement("name");
        exerciseElem.appendChild(exerciseNameElem);
        QDomText exerciseNameText = doc.createTextNode(exercise->m_name);
        exerciseNameElem.appendChild(exerciseNameText);

        QDomElement needSatisfactionsElem = doc.createElement("needSatisfactions");
        exerciseElem.appendChild(needSatisfactionsElem);
        QDomElement aaElem = doc.createElement("aa");
        QDomText aaElemText = doc.createTextNode(QString::number(exercise->m_aa));
        aaElem.appendChild(aaElemText);
        needSatisfactionsElem.appendChild(aaElem);
        QDomElement bbElem = doc.createElement("bb");
        QDomText bbElemText = doc.createTextNode(QString::number(exercise->m_bb));
        bbElem.appendChild(bbElemText);
        needSatisfactionsElem.appendChild(bbElem);
        QDomElement ccElem = doc.createElement("cc");
        QDomText ccElemText = doc.createTextNode(QString::number(exercise->m_cc));
        ccElem.appendChild(ccElemText);
        needSatisfactionsElem.appendChild(ccElem);

        QDomElement exerciseDurationElem = doc.createElement("duration");
        QDomText exerciseDurationElemText = doc.createTextNode(QString::number(exercise->m_duration));
        exerciseDurationElem.appendChild(exerciseDurationElemText);
        exerciseElem.appendChild(exerciseDurationElem);
    }

    QString xml = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n" + doc.toString();

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "Error! could not open file.";
        return;
    }
    QTextStream stream(&file);
    stream << xml;

    file.close();
}

void MainWindow::loadModelFromDocument(QDomDocument* _doc)
{
    m_players.clear();
    m_exercises.clear();

    QDomElement modelElem = _doc->documentElement();

    QDomNode playersNode = modelElem.firstChild();
    QDomNode playerNode = playersNode.firstChild();
    while (!playerNode.isNull()) {
        QDomNode nameNode = playerNode.firstChild();
        QString name = nameNode.toElement().text();

        QDomNode needsNode = nameNode.nextSibling();
        QDomNode needNode = needsNode.firstChild();
        int a = needNode.toElement().text().toInt();
        int b = needNode.nextSibling().toElement().text().toInt();
        int c = needNode.nextSibling().toElement().text().toInt();

        QVector<QString> startTime;
        QVector<QString> endTime;

        QDomNode scheduleNode = needsNode.nextSibling();
        QDomNode mondayNode = scheduleNode.firstChild();
        QDomNode mondayStartNode = mondayNode.firstChild();
        QString mondayStart = mondayStartNode.toElement().text();
        QString mondayEnd = mondayStartNode.nextSibling().toElement().text();
        startTime.append(mondayStart);
        endTime.append(mondayEnd);

        QDomNode wednesdayNode = mondayNode.nextSibling();
        QDomNode wednesdayStartNode = wednesdayNode.firstChild();
        QString wednesdayStart = wednesdayStartNode.toElement().text();
        QString wednesdayEnd = wednesdayStartNode.nextSibling().toElement().text();
        startTime.append(wednesdayStart);
        endTime.append(wednesdayEnd);

        QDomNode fridayNode = wednesdayNode.nextSibling();
        QDomNode fridayStartNode = fridayNode.firstChild();
        QString fridayStart = fridayStartNode.toElement().text();
        QString fridayEnd = fridayStartNode.nextSibling().toElement().text();
        startTime.append(fridayStart);
        endTime.append(fridayEnd);

        m_players.append(new Player(name, a, b, c, startTime, endTime));

        playerNode = playerNode.nextSibling();
    }

    QDomNode exercisesNode = playersNode.nextSibling();
    QDomNode exerciseNode = exercisesNode.firstChild();
    while (!exerciseNode.isNull()) {
        QDomNode nameNode = exerciseNode.firstChild();
        QString name = nameNode.toElement().text();

        QDomNode needSatisfactionsNode = nameNode.nextSibling();
        QDomNode needSatisfactionNode = needSatisfactionsNode.firstChild();
        int aa = needSatisfactionNode.toElement().text().toInt();
        int bb = needSatisfactionNode.nextSibling().toElement().text().toInt();
        int cc = needSatisfactionNode.nextSibling().toElement().text().toInt();

        int duration = needSatisfactionsNode.nextSibling().toElement().text().toInt();

        m_exercises.append(new Exercise(name, aa, bb, cc, duration));

        exerciseNode = exerciseNode.nextSibling();
    }
}

void MainWindow::updateUI()
{
    disconnectContextUpdateSignals();

    ui.saveListWidget->clear();

    auto isPlayersView = ui.playersRadioButton->isChecked();
    Q_ASSERT((ui.playersRadioButton->isChecked() && !ui.tasksRadioButton->isChecked()) ||
             (!ui.playersRadioButton->isChecked() && ui.tasksRadioButton->isChecked()));

    ui.taskDurationSpinBox->setVisible(!isPlayersView);
    ui.taskDurationLabel->setVisible(!isPlayersView);
    ui.isPresentWidget->setVisible(isPlayersView);

    if (isPlayersView) {
        for (auto player : m_players) {
            QListWidgetItem* playerItem = new QListWidgetItem();
            playerItem->setText(player->m_name);
            ui.saveListWidget->addItem(playerItem);
            ui.saveListWidget->setCurrentItem(playerItem);
        }

        if (!m_previousIndex.isValid())
            m_previousIndex = ui.saveListWidget->currentIndex();
        else
            ui.saveListWidget->setCurrentIndex(m_previousIndex);

        for (auto player : m_players) {
            if (player->m_name == ui.saveListWidget->itemFromIndex(m_previousIndex)->text()) {
                ui.aSlider->setValue(player->m_a);
                ui.bSlider->setValue(player->m_b);
                ui.cSlider->setValue(player->m_c);
                m_isPresentWidget->updateUI(player);
            }
        }
        
    }
    else {
        for (auto exercise : m_exercises) {
            QListWidgetItem* exerciseItem = new QListWidgetItem();
            exerciseItem->setText(exercise->m_name);
            ui.saveListWidget->addItem(exerciseItem);
            ui.saveListWidget->setCurrentItem(exerciseItem);
        }

        if (!m_previousIndex.isValid())
            m_previousIndex = ui.saveListWidget->currentIndex();
        else
            ui.saveListWidget->setCurrentIndex(m_previousIndex);

        for (auto exercise : m_exercises) {
            if (exercise->m_name == ui.saveListWidget->itemFromIndex(m_previousIndex)->text()) {
                ui.aSlider->setValue(exercise->m_aa);
                ui.bSlider->setValue(exercise->m_bb);
                ui.cSlider->setValue(exercise->m_cc);
                ui.taskDurationSpinBox->setValue(exercise->m_duration);
            }
        }
    }

    m_wasPlayersView = ui.playersRadioButton->isChecked();

    connectContextUpdateSignals();
}

void MainWindow::onObjectAdd()
{
    ObjectAddDialog _objectAddDialog(this);
    if (_objectAddDialog.exec() == QDialog::Accepted) {
        auto isPlayersView = ui.playersRadioButton->isChecked();
        Q_ASSERT((ui.playersRadioButton->isChecked() && !ui.tasksRadioButton->isChecked()) ||
            (!ui.playersRadioButton->isChecked() && ui.tasksRadioButton->isChecked()));

        if (isPlayersView) {
            Player* player = new Player(_objectAddDialog.getObjectName());
            m_players.append(player);
        }
        else {
            Exercise* exercise = new Exercise(_objectAddDialog.getObjectName());
            m_exercises.append(exercise);
        }

        updateContextAndUI();
    }
}

void MainWindow::onObjectDelete()
{
    auto object = ui.saveListWidget->currentItem();

    auto isPlayersView = ui.playersRadioButton->isChecked();
    Q_ASSERT((ui.playersRadioButton->isChecked() && !ui.tasksRadioButton->isChecked()) ||
        (!ui.playersRadioButton->isChecked() && ui.tasksRadioButton->isChecked()));

    if (isPlayersView) {
        for (int i = 0; i < m_players.size(); ++i) {
            if (m_players[i]->m_name == object->text()) {
                m_players.removeAt(i);
            }
        }
    }
    else {
        for (int i = 0; i < m_exercises.size(); ++i) {
            if (m_exercises[i]->m_name == object->text()) {
                m_exercises.removeAt(i);
            }
        }
    }

    delete object;
    m_previousIndex = ui.saveListWidget->currentIndex();

    updateUI();
}

void MainWindow::updateObjectContext()
{
    if (!m_previousIndex.isValid())
        return;

    if (m_wasPlayersView) {
        for (auto player : m_players) {
            if (player->m_name == ui.saveListWidget->itemFromIndex(m_previousIndex)->text()) {
                player->m_a = ui.aSlider->value();
                player->m_b = ui.bSlider->value();
                player->m_c = ui.cSlider->value();
                m_isPresentWidget->updatePlayer(player);
            }
        }
    }
    else {
        for (auto exercise : m_exercises) {
            if (exercise->m_name == ui.saveListWidget->itemFromIndex(m_previousIndex)->text()) {
                exercise->m_aa = ui.aSlider->value();
                exercise->m_bb = ui.bSlider->value();
                exercise->m_cc = ui.cSlider->value();
                exercise->m_duration = ui.taskDurationSpinBox->value();
            }
        }
    }

    if (m_wasPlayersView != ui.playersRadioButton->isChecked())
        m_previousIndex = QModelIndex();
    else
        m_previousIndex = ui.saveListWidget->currentIndex();

    m_wasPlayersView = ui.playersRadioButton->isChecked();
}

void MainWindow::updateContextAndUI()
{
    updateObjectContext();
    updateUI();
}

void MainWindow::onActionOpen()
{
    QString path = "";

    path = QFileDialog::getOpenFileName(
        this, 
        "Browse to model data file", 
        QDir::currentPath(), 
        "XML files (*.xml);;Text files (*.txt)", 
        0, 
        QFileDialog::ReadOnly);

    if (!path.isEmpty()) {
        try {
            QDomDocument* doc = new QDomDocument("mydocument");
            QFile file(path);
            if (!file.open(QIODevice::ReadOnly))
                throw "Error";
            if (!doc->setContent(&file)) {
                file.close();
                throw "Error";
            }
            file.close();

            loadModelFromDocument(doc);
            updateUI();
            m_wasPlayersView = ui.playersRadioButton->isChecked();
            m_previousIndex = ui.saveListWidget->currentIndex();

            ui.statusBar->showMessage("Model parameteres were loaded successfully!");
        } 
        catch (...) {
            updateUI();
            ui.statusBar->showMessage("Couldn't parse model file");
        }

    }
}
