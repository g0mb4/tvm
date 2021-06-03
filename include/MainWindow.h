#pragma once

#include <stdio.h>

#include <memory>
#include <string>
#include <thread>

#include <QFileDialog>
#include <QMainWindow>
#include <QMessageBox>
#include <QTimer>

#include "Bus.h"
#include "CPU.h"
#include "Display.h"
#include "Helpers.h"
#include "Memory.h"
#include "Stack.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget* parent, int argc, char** argv);
    ~MainWindow();

private:
    std::shared_ptr<CPU> m_cpu;
    std::shared_ptr<Memory> m_memory;
    std::shared_ptr<Bus> m_bus;
    std::shared_ptr<Display> m_display;
    std::shared_ptr<Stack> m_stack;
    std::unique_ptr<QTimer> m_timer;

    void parse_args(int argc, char** argv);

    void load_program(const uint8_t* data, uint32_t len);

    void update_memory_view(uint32_t start_address = 0);
    void update_cpu_view();
    void update_display();
    void update_status_bar();

    void reset();
    void update_ui();

    void step();
    void run();
    bool m_running { false };

    void btn_open_file();
    void open_file(const char* file_name);

    Ui::MainWindow* ui;
};
