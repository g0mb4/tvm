#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent, int argc, char ** argv)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_bus = std::make_shared<Bus>();
    m_memory = std::make_shared<Memory>();
    m_display = std::make_shared<Display>();
    m_cpu = std::make_shared<CPU>(m_bus);

    m_bus->add(m_memory);
    m_bus->add(m_display);

    connect(ui->actionQuit, &QAction::triggered, this, [this]{close();});
    connect(ui->btn_step, &QPushButton::clicked, this, &MainWindow::step);
    connect(ui->btn_reset, &QPushButton::clicked, this, [this]{ m_cpu->reset(); m_display->reset(); update_ui();});
    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::btn_open_file);

    parse_args(argc, argv);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::parse_args(int argc, char ** argv){
    char * file_name = nullptr;

    for(int a = 1; a < argc; a++){
        if(strcmp(argv[a], "-s") == 0 || strcmp(argv[a], "--start") == 0){
            // start program
        } else {
            file_name = strdup(argv[a]);
        }
    }

    if(file_name){
        open_file(file_name);
        free(file_name);
    }
}

void MainWindow::reset(){
    m_cpu->reset();
    m_display->reset();
}

void MainWindow::update_ui(){
    update_memory_view();
    update_cpu_view();
    update_status_bar();
    update_display();
}

void MainWindow::load_program(const uint8_t * data, uint32_t len){
    m_memory->load(data, len);
    reset();
    update_ui();
}

void MainWindow::update_memory_view(uint32_t start_address){
    const uint16_t * memory = m_memory->data();
    std::string content;

    // FIXME: fix this on windows
    for(int i = start_address; i < Memory::size; i++){
        if(i % 5 == 0){
            content += Helpers::value_to_hex_string(i) + ": ";
        }

        content += Helpers::value_to_hex_string(memory[i]) + " ";
    }

    ui->te_memory_view->setText(QString::fromStdString(content));
}

void MainWindow::update_cpu_view(){
    const uint16_t * regs = m_cpu->registers();

    ui->tb_r0->setText(QString::fromStdString(Helpers::value_to_hex_string(regs[0])));
    ui->tb_r1->setText(QString::fromStdString(Helpers::value_to_hex_string(regs[1])));
    ui->tb_r2->setText(QString::fromStdString(Helpers::value_to_hex_string(regs[2])));
    ui->tb_r3->setText(QString::fromStdString(Helpers::value_to_hex_string(regs[3])));
    ui->tb_r4->setText(QString::fromStdString(Helpers::value_to_hex_string(regs[4])));
    ui->tb_r5->setText(QString::fromStdString(Helpers::value_to_hex_string(regs[5])));
    ui->tb_r6->setText(QString::fromStdString(Helpers::value_to_hex_string(regs[6])));
    ui->tb_r7->setText(QString::fromStdString(Helpers::value_to_hex_string(regs[7])));

    ui->tb_pc->setText(QString::fromStdString(Helpers::value_to_hex_string(m_cpu->program_counter())));
    ui->tb_sp->setText(QString::fromStdString(Helpers::value_to_hex_string(m_cpu->stack_pointer())));

    ui->cb_carry->setChecked(m_cpu->carry_flag());
    ui->cb_zero->setChecked(m_cpu->zero_flag());
}

void MainWindow::update_display(){
    ui->txb_display->setText(QString::fromStdString(m_display->message()));
}

void MainWindow::update_status_bar(){
    auto instruction = m_cpu->current_instruction();
    if(instruction){
        ui->statusbar->showMessage(QString::fromStdString(instruction->name()));
    }
}

void MainWindow::step(){
    if(m_cpu->is_halted()){
        return;
    }

    m_cpu->step();

    update_ui();

    if(m_cpu->has_error()){
        QMessageBox msgbox;
        msgbox.critical(0, "Error", QString::fromStdString(m_cpu->error_string()));
    }
}

void MainWindow::btn_open_file(){
    QString file_name = QFileDialog::getOpenFileName(this, "Open Binary File", "", "Binary file (*.bin)");

    if(file_name.isEmpty()){
        return;
    } else {
        open_file(file_name.toStdString().c_str());
    }
}

void MainWindow::open_file(const char * file_name){
    FILE * fp = nullptr;

    fp = fopen(file_name, "rb");

    if(fp == nullptr){
        QMessageBox msgbox;
        msgbox.critical(0, "Error", "Unable to open the file: " + QString(file_name));
        return;
    }

    uint8_t byte;
    std::vector<uint8_t> data;

    while(fread(&byte, sizeof(uint8_t), 1, fp) == 1){
        data.emplace_back(byte);
    }

    fclose(fp);

    load_program(data.data(), data.size());
}

