/****************************************************************************
**
** Copyright 2020, Prashanth N Udupa <prashanth.udupa@gmail.com>
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
**
** 1. Redistributions of source code must retain this copyright
** notice, this list of conditions and the following disclaimer.
**
** 2. Redistributions in binary form must reproduce this copyright
** notice, this list of conditions and the following disclaimer in the
** documentation and/or other materials provided with the distribution.
**
** 3. Neither the name of the copyright holder nor the names of its
** contributors may be used to endorse or promote products derived from
** this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
** CONTRIBUTORS “AS IS” AND ANY EXPRESS OR IMPLIED WARRANTIES,
** INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
** MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
** IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
** ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
** CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
** SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
** BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
** WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
** OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
** EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
****************************************************************************/

#include "anchorlayout.h"
#include <QtWidgets>

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
            ->anchorTo(containerLayout->customLine(Qt::Vertical, 0.15))
            ->setMargin(-5);
    frame1Layout->right()
            ->anchorTo(containerLayout->horizontalCenter())
            ->setMargin(5);
    frame1Layout->top()->anchorTo(containerLayout->top())->setMargin(10);
    frame1Layout->bottom()
            ->anchorTo(containerLayout->customLine(Qt::Horizontal, -0.25))
            ->setMargin(-10);

    AnchorLayout *frame2Layout = AnchorLayout::get(frame2);
    frame2Layout->right()
            ->anchorTo(containerLayout->customLine(Qt::Vertical, -0.15))
            ->setMargin(-5);
    frame2Layout->left()
            ->anchorTo(containerLayout->horizontalCenter())
            ->setMargin(5);
    frame2Layout->top()
            ->anchorTo(containerLayout->customLine(Qt::Horizontal, 0.25))
            ->setMargin(-10);
    frame2Layout->bottom()->anchorTo(containerLayout->bottom())->setMargin(10);

    container.resize(600, 400);
    container.show();

    return a.exec();
}
