#pragma once

#include <stdio.h>

#include <string>
#include <memory>

#include <QMainWindow>
#include <QMessageBox>
#include <QFileDialog>

#include "Helpers.h"
#include "CPU.h"
#include "Memory.h"
#include "Bus.h"
#include "Display.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    std::shared_ptr<CPU> m_cpu;
    std::shared_ptr<Memory> m_memory;
    std::shared_ptr<Bus> m_bus;
    std::shared_ptr<Display> m_display;

    void load_program(const uint8_t * data, uint32_t len);

    void update_memory_view(uint32_t start_address = 0);
    void update_cpu_view();
    void update_display();
    void update_status_bar();

    void reset();
    void update_ui();

    void step();

    void btn_open_file();
    void open_file(const char * file_name);

    Ui::MainWindow *ui;
};
