#include <QApplication>
#include <QWidget>
#include <QGridLayout>
#include <QPushButton>
#include <QLineEdit>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QWidget window;
    window.resize(150, 150);
    window.setWindowTitle("Example calculator");

    auto memory = 0.0;
    auto op = '\0';

    auto grid = new QVBoxLayout(&window);
    auto output_text = new QLineEdit{&window};
    output_text->setObjectName("output");
    output_text->setReadOnly(true);
    output_text->setText("");
    grid->addWidget(output_text);

    auto *btn_grid = new QGridLayout(&window);
    static const auto button_keys = QVector<QVector<QString>>({
        {"7", "8", "9", "/"},
        {"4", "5", "6", "*"},
        {"1", "2", "3", "-"},
        {"0", "", "=", "+"},
    });
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            auto const& text = button_keys[i][j];
            auto *btn = new QPushButton(text, &window);
            btn->setObjectName("btn_" + text);
            btn->setFixedSize(30, 30);
            QObject::connect(btn, &QPushButton::clicked, [&op, &memory, &output_text, text](){
                if (text == "") {
                    output_text->setText("");
                } else if (text == "+" || text == "-" || text == "/" || text == "*") {
                    memory = output_text->text().toDouble();
                    op = text.toStdString()[0];
                    output_text->setText("");
                } else if (text == "=") {
                    if (op == '\0') {
                        return;
                    }

                    auto result = 0.0;
                    auto rhs = output_text->text().toDouble();
                    if (op == '+') { result = memory + rhs; }
                    if (op == '-') { result = memory - rhs; }
                    if (op == '/') { result = memory / rhs; }
                    if (op == '*') { result = memory * rhs; }

                    memory = 0.0;
                    op = '\0';
                    output_text->setText(QString::number(result));
                } else {
                    auto result = (output_text->text() + text).toDouble();
                    output_text->setText(QString::number(result));
                }
            });
            btn_grid->addWidget(btn, i, j);
        }
    }
    grid->addLayout(btn_grid);
    window.setLayout(grid);

    window.show();
    return app.exec();
}
