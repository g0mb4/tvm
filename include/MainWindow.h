#pragma once

#include <stdio.h>

#include <string>
#include <memory>

#include <QMainWindow>
#include <QMessageBox>

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
    void update_status_bar();

    void reset();
    void update_ui();

    void step();

    std::string value_to_hex_string(uint16_t value);

    static constexpr uint8_t test_program[] = {0x21, 0x02, 0x14, 0x00, 0x22, 0x62, 0x0d, 0x00, 0x08, 0x90, 0x0c, 0x00, 0x2a, 0xc0, 0x21, 0x30,
                                               0x01, 0x00, 0x22, 0x70, 0x18, 0xa0, 0xfa, 0xff, 0x00, 0xf0, 0x61, 0x00, 0x62, 0x00, 0x63, 0x00,
                                               0x64, 0x00, 0x65, 0x00, 0x66, 0x00, 0x00, 0x00, 0x06, 0x00};
    static constexpr uint32_t test_program_size = sizeof(test_program);

    Ui::MainWindow *ui;
};
