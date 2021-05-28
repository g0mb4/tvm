#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_memory = std::make_shared<Memory>();
    m_bus = std::make_shared<Bus>(m_memory);
    m_cpu = std::make_shared<CPU>(m_bus);

    load_program(MainWindow::test_program, MainWindow::test_program_size);

    connect(ui->actionQuit, &QAction::triggered, this, [this]{close();});
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::load_program(const uint8_t * data, uint32_t len){
    m_memory->load(data, len);
    m_cpu->reset();

    update_memory_view();
    update_cpu_view();
}

std::string MainWindow::value_to_hex_string(uint16_t value){
    char hex[5];
    snprintf(hex, sizeof(hex), "%04X", value);
    return std::string(hex);
}

void MainWindow::update_memory_view(uint32_t start_address){
    const uint16_t * memory = m_memory->data();
    std::string content;

    for(int i = start_address; i < Memory::size; i++){
        if(i % 5 == 0){
            content += value_to_hex_string(i) + ": ";
        }

        content += value_to_hex_string(memory[i]) + " ";
    }

    ui->te_memory_view->setText(QString::fromStdString(content));
}

void MainWindow::update_cpu_view(){
    const uint16_t * regs = m_cpu->registers();

    ui->tb_r0->setText(QString::fromStdString(value_to_hex_string(regs[0])));
    ui->tb_r1->setText(QString::fromStdString(value_to_hex_string(regs[1])));
    ui->tb_r2->setText(QString::fromStdString(value_to_hex_string(regs[2])));
    ui->tb_r3->setText(QString::fromStdString(value_to_hex_string(regs[3])));
    ui->tb_r4->setText(QString::fromStdString(value_to_hex_string(regs[4])));
    ui->tb_r5->setText(QString::fromStdString(value_to_hex_string(regs[5])));
    ui->tb_r6->setText(QString::fromStdString(value_to_hex_string(regs[6])));
    ui->tb_r7->setText(QString::fromStdString(value_to_hex_string(regs[7])));

    ui->tb_pc->setText(QString::fromStdString(value_to_hex_string(m_cpu->program_counter())));
    ui->tb_sp->setText(QString::fromStdString(value_to_hex_string(m_cpu->stack_pointer())));

    ui->cb_carry->setChecked(m_cpu->carry_flag());
    ui->cb_zero->setChecked(m_cpu->zero_flag());
}

