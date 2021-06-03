#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget* parent, int argc, char** argv)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_bus = std::make_shared<Bus>();
    m_memory = std::make_shared<Memory>();
    m_display = std::make_shared<Display>();
    m_stack = std::make_shared<Stack>(m_memory);

    m_cpu = std::make_shared<CPU>(m_bus, m_stack);

    m_bus->add(m_memory);
    m_bus->add(m_display);

    m_timer = std::make_unique<QTimer>(this);

    connect(ui->actionQuit, &QAction::triggered, this, [this] { close(); });
    connect(ui->btn_step, &QPushButton::clicked, this, &MainWindow::step);
    connect(ui->btn_run, &QPushButton::clicked, this, &MainWindow::run);
    connect(ui->btn_stop, &QPushButton::clicked, this,
        [this] { m_running = false; });
    connect(ui->btn_reset, &QPushButton::clicked, this, [this] { reset(); });
    connect(ui->actionOpen, &QAction::triggered, this,
        &MainWindow::btn_open_file);

    connect(m_timer.get(), &QTimer::timeout, this, &MainWindow::update_ui);

    m_timer->start(33);

    parse_args(argc, argv);
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::parse_args(int argc, char** argv)
{
    char* file_name = nullptr;
    bool autostart = false;

    for (int a = 1; a < argc; a++) {
        if (strcmp(argv[a], "-s") == 0 || strcmp(argv[a], "--start") == 0) {
            autostart = true;
        } else {
            file_name = strdup(argv[a]);
        }
    }

    if (file_name) {
        open_file(file_name);
        free(file_name);

        if (autostart) {
            run();
        }
    }
}

void MainWindow::reset()
{
    m_cpu->reset();
    m_display->reset();

    m_memory->soft_reset();
}

void MainWindow::update_ui()
{
    bool ok;
    uint32_t memory_view_start = ui->txb_memory_start->text().toUInt(&ok, 16);
    if (ok) {
        if (memory_view_start >= Memory::size) {
            QMessageBox msgbox;
            msgbox.critical(0, "Error", "Memory address is too large.");
            ui->txb_memory_start->setText("0");
            return;
        }

        update_memory_view(memory_view_start);
    }

    update_stack_view();
    update_cpu_view();
    update_status_bar();
    update_display();

    if (m_cpu->has_error()) {
        QMessageBox msgbox;
        msgbox.critical(0, "Error", QString::fromStdString(m_cpu->error_string()));
        m_cpu->clear_error();
    }
}

void MainWindow::update_memory_view(uint32_t start_address)
{
    const uint16_t* memory = m_memory->data();
    std::string content;

    int number_of_lines = 0;
    for (int i = start_address, j = 0; i < Memory::size; i++, j++) {
        if (j % 6 == 0) {
            if (number_of_lines > 11) {
                break;
            }

            if (j == 0) {
                content += Helpers::value_to_hex_string(i) + ": ";
            } else {
                content += '\n' + Helpers::value_to_hex_string(i) + ": ";
            }

            number_of_lines++;
        }

        content += Helpers::value_to_hex_string(memory[i]) + ' ';
    }

    ui->te_memory_view->setText(QString::fromStdString(content));
}

void MainWindow::update_stack_view()
{
    const uint16_t* memory = m_memory->data();
    std::string content;

    for (uint32_t i = Stack::end + 1; i <= Stack::start; i++) {
        if (i == m_cpu->stack_pointer()) {
            content += Helpers::value_to_hex_string(i) + ": " + Helpers::value_to_hex_string(memory[i]) + " <--\n";
        } else {
            content += Helpers::value_to_hex_string(i) + ": " + Helpers::value_to_hex_string(memory[i]) + "\n";
        }
    }

    ui->te_stack_view->setText(QString::fromStdString(content));
}

void MainWindow::update_cpu_view()
{
    const uint16_t* regs = m_cpu->registers();

    ui->tb_r0->setText(
        QString::fromStdString(Helpers::value_to_hex_string(regs[0])));
    ui->tb_r1->setText(
        QString::fromStdString(Helpers::value_to_hex_string(regs[1])));
    ui->tb_r2->setText(
        QString::fromStdString(Helpers::value_to_hex_string(regs[2])));
    ui->tb_r3->setText(
        QString::fromStdString(Helpers::value_to_hex_string(regs[3])));
    ui->tb_r4->setText(
        QString::fromStdString(Helpers::value_to_hex_string(regs[4])));
    ui->tb_r5->setText(
        QString::fromStdString(Helpers::value_to_hex_string(regs[5])));
    ui->tb_r6->setText(
        QString::fromStdString(Helpers::value_to_hex_string(regs[6])));
    ui->tb_r7->setText(
        QString::fromStdString(Helpers::value_to_hex_string(regs[7])));

    ui->tb_pc->setText(QString::fromStdString(
        Helpers::value_to_hex_string(m_cpu->program_counter())));
    ui->tb_sp->setText(QString::fromStdString(
        Helpers::value_to_hex_string(m_cpu->stack_pointer())));

    ui->cb_carry->setChecked(m_cpu->carry_flag());
    ui->cb_zero->setChecked(m_cpu->zero_flag());
}

void MainWindow::update_display()
{
    ui->txb_display->setText(QString::fromStdString(m_display->message()));
}

void MainWindow::update_status_bar()
{
    auto instruction = m_cpu->current_instruction();
    if (instruction) {
        ui->statusbar->showMessage(QString::fromStdString(instruction->name()));
    }
}

void MainWindow::step()
{
    if (m_cpu->is_halted()) {
        return;
    }

    m_cpu->step();
}

void MainWindow::btn_open_file()
{
    QString file_name = QFileDialog::getOpenFileName(this, "Open Binary File", "",
        "Binary file (*.bin)");

    if (file_name.isEmpty()) {
        return;
    } else {
        open_file(file_name.toStdString().c_str());
    }
}

void MainWindow::open_file(const char* file_name)
{
    FILE* fp = nullptr;

    fp = fopen(file_name, "rb");

    if (fp == nullptr) {
        QMessageBox msgbox;
        msgbox.critical(0, "Error",
            "Unable to open the file: " + QString(file_name));
        return;
    }

    uint8_t byte;
    std::vector<uint8_t> data;

    while (fread(&byte, sizeof(uint8_t), 1, fp) == 1) {
        data.emplace_back(byte);
    }

    fclose(fp);

    const QFileInfo info(file_name);

    setWindowTitle(QString::fromStdString(std::string("tvm - ")) + info.baseName());

    load_program(data.data(), data.size());
}

void MainWindow::load_program(const uint8_t* data, uint32_t len)
{
    m_memory->load(data, len);

    if (m_memory->has_error()) {
        QMessageBox msgbox;
        m_memory->reset();
        msgbox.critical(0, "Error",
            QString::fromStdString(m_memory->error_string()));
    } else {
        reset();
    }
}

void MainWindow::run()
{
    m_running = true;

    reset();

    std::unique_ptr<std::thread> worker = std::make_unique<std::thread>([this] {
        while (m_running && !m_cpu->is_halted()) {
            step();

            if (m_cpu->has_error()) {
                m_running = false;
            }
        }
    });

    worker->detach();
}
