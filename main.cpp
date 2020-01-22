#include <QtWidgets>
#include "anchorlayout.h"

int main(int argc, char **argv)
{
    QApplication a(argc, argv);

    QWidget container;

    QLabel *frame1 = new QLabel(&container);
    frame1->setFrameStyle(QFrame::Box);
    frame1->setText("One");
    frame1->setAlignment(Qt::AlignCenter);

    QLabel *frame2 = new QLabel(&container);
    frame2->setFrameStyle(QFrame::Box);
    frame2->setText("Two");
    frame2->setAlignment(Qt::AlignCenter);

    AnchorLayout *containerLayout = AnchorLayout::get(&container);

    AnchorLayout *frame1Layout = AnchorLayout::get(frame1);
    frame1Layout->left()
                ->anchorTo(containerLayout->customLine(Qt::Vertical,0.15))
                ->setMargin(-5);
    frame1Layout->right()
                ->anchorTo(containerLayout->horizontalCenter())
                ->setMargin(5);
    frame1Layout->top()
                ->anchorTo(containerLayout->top())
                ->setMargin(10);
    frame1Layout->bottom()
                ->anchorTo(containerLayout->customLine(Qt::Horizontal,-0.25))
                ->setMargin(-10);

    AnchorLayout *frame2Layout = AnchorLayout::get(frame2);
    frame2Layout->right()
                ->anchorTo(containerLayout->customLine(Qt::Vertical,-0.15))
                ->setMargin(-5);
    frame2Layout->left()
                ->anchorTo(containerLayout->horizontalCenter())
                ->setMargin(5);
    frame2Layout->top()
                ->anchorTo(containerLayout->customLine(Qt::Horizontal,0.25))
                ->setMargin(-10);
    frame2Layout->bottom()
                ->anchorTo(containerLayout->bottom())
                ->setMargin(10);

    container.resize(600, 400);
    container.show();

    return a.exec();
}
