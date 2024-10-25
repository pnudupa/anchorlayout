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

#include <QEvent>
#include <QtDebug>

AnchorLayout *AnchorLayout::get(QWidget *widget)
{
    if (widget == nullptr)
        return nullptr;

    AnchorLayout *layout = widget->findChild<AnchorLayout *>(
            QString(), Qt::FindDirectChildrenOnly);
    if (layout == nullptr)
        layout = new AnchorLayout(widget);

    return layout;
}

AnchorLayout::AnchorLayout(QWidget *widget) : QObject(widget), m_widget(widget)
{
    widget->installEventFilter(this);
    m_margins = 0;

    m_leftLine = nullptr;
    m_topLine = nullptr;
    m_bottomLine = nullptr;
    m_rightLine = nullptr;
    m_hcenterLine = nullptr;
    m_vcenterLine = nullptr;
}

AnchorLayout::~AnchorLayout() { }

#define FETCH_ANCHOR_LINE(x, e)                                                \
    if (x == nullptr) {                                                        \
        x = new AnchorLine(this, e);                                           \
        x->setMargin(m_margins);                                               \
    }                                                                          \
    return x;

AnchorLine *AnchorLayout::left() { FETCH_ANCHOR_LINE(m_leftLine,
                                                     AnchorLine::LeftEdge) }

AnchorLine *AnchorLayout::top() { FETCH_ANCHOR_LINE(m_topLine,
                                                    AnchorLine::TopEdge) }

AnchorLine *AnchorLayout::right() { FETCH_ANCHOR_LINE(m_rightLine,
                                                      AnchorLine::RightEdge) }

AnchorLine *AnchorLayout::bottom() { FETCH_ANCHOR_LINE(m_bottomLine,
                                                       AnchorLine::BottomEdge) }

AnchorLine *AnchorLayout::horizontalCenter() {
    FETCH_ANCHOR_LINE(m_hcenterLine, AnchorLine::HCenter)
}

AnchorLine *AnchorLayout::verticalCenter() {
    FETCH_ANCHOR_LINE(m_vcenterLine, AnchorLine::VCenter)
}

AnchorLine *AnchorLayout::customLine(Qt::Orientation orientation, qreal percent,
                                     OffsetDirection offsetDirection)
{
    const AnchorLine::Edge edge = (orientation == Qt::Horizontal)
            ? AnchorLine::Horizontal
            : AnchorLine::Vertical;
    AnchorLine *line = new AnchorLine(this, edge, percent);
    if (offsetDirection == OD_Auto)
        offsetDirection = percent < 0 ? OD_Left : OD_Right;
    line->setOffsetDirection(offsetDirection);
    m_customLines.append(line);
    return line;
}

void AnchorLayout::centerIn(AnchorLayout *other)
{
    if (!this->isAnchorAllowed(other))
        return;

    if (m_leftLine)
        m_leftLine->anchorTo(nullptr);

    if (m_topLine)
        m_topLine->anchorTo(nullptr);

    if (m_rightLine)
        m_rightLine->anchorTo(nullptr);

    if (m_bottomLine)
        m_bottomLine->anchorTo(nullptr);

    if (other != nullptr) {
        this->horizontalCenter()->anchorTo(other->horizontalCenter());
        this->verticalCenter()->anchorTo(other->verticalCenter());
    } else {
        if (m_hcenterLine)
            m_hcenterLine->anchorTo(nullptr);

        if (m_vcenterLine)
            m_vcenterLine->anchorTo(nullptr);
    }
}

AnchorLayout *AnchorLayout::fill(AnchorLayout *other)
{
    if (!this->isAnchorAllowed(other))
        return this;

    if (other != nullptr) {
        this->left()->anchorTo(other->left());
        this->right()->anchorTo(other->right());
        this->top()->anchorTo(other->top());
        this->bottom()->anchorTo(other->bottom());
    } else {
        if (m_leftLine)
            m_leftLine->anchorTo(nullptr);

        if (m_topLine)
            m_topLine->anchorTo(nullptr);

        if (m_rightLine)
            m_rightLine->anchorTo(nullptr);

        if (m_bottomLine)
            m_bottomLine->anchorTo(nullptr);
    }

    if (m_hcenterLine)
        m_hcenterLine->anchorTo(nullptr);

    if (m_vcenterLine)
        m_vcenterLine->anchorTo(nullptr);

    return this;
}

void AnchorLayout::setMargins(int margin)
{
    if (m_margins == margin)
        return;

    if (m_leftLine)
        m_leftLine->setMargin(margin);

    if (m_topLine)
        m_topLine->setMargin(margin);

    if (m_rightLine)
        m_rightLine->setMargin(margin);

    if (m_bottomLine)
        m_bottomLine->setMargin(margin);

    m_margins = margin;
}

void AnchorLayout::update()
{
    m_updateTimer.start(0, this);
}

bool AnchorLayout::eventFilter(QObject *object, QEvent *event)
{
    if (object == m_widget) {
        switch (event->type()) {
        case QEvent::Move:
        case QEvent::Resize:
            emit geometryChanged(m_widget->geometry());
            this->update();
            break;
        default:
            break;
        }
    }

    return false;
}

void AnchorLayout::timerEvent(QTimerEvent *te)
{
    if (m_updateTimer.timerId() != te->timerId())
        return;

    m_updateTimer.stop();

    AnchorLine *lines[6] = { m_leftLine,   m_topLine,     m_rightLine,
                             m_bottomLine, m_vcenterLine, m_hcenterLine };
    for (int i = 0; i < 6; i++) {
        if (lines[i])
            lines[i]->update();
    }

    Q_FOREACH (AnchorLine *line, m_customLines)
        line->update();
}

bool AnchorLayout::isAnchorAllowed(AnchorLine *line) const
{
    if (line == nullptr)
        return false;

    return this->isAnchorAllowed(line->layout());
}

bool AnchorLayout::isAnchorAllowed(AnchorLayout *layout) const
{
    if (layout == nullptr)
        return false;

    return layout->widget() == m_widget->parentWidget()
            || layout->widget()->parentWidget() == m_widget->parentWidget();
}

///////////////////////////////////////////////////////////////////////////////

AnchorLine::AnchorLine(AnchorLayout *layout, AnchorLine::Edge edge,
                       qreal percent)
    : QObject(layout),
      m_layout(layout),
      m_edge(edge),
      m_percent(0.0),
      m_offset(0),
      m_anchoredTo(nullptr)
{
    if (edge == Horizontal || edge == Vertical) {
        qreal pc = qAbs(percent);
        pc = qMax(qMin(pc, 1.0), 0.0);
        if (percent < 0)
            m_percent = 1.0 - pc;
        else
            m_percent = pc;
    }

    switch (m_edge) {
    case LeftEdge:
    case TopEdge:
        m_offsetDirection = 1;
        break;
    case RightEdge:
    case BottomEdge:
        m_offsetDirection = -1;
        break;
    default:
        m_offsetDirection = 1;
    }
}

AnchorLine::~AnchorLine()
{
    if (m_anchoredTo != nullptr)
        m_anchoredTo->removeFromUpdateList(this);
    m_anchoredTo = nullptr;

    QList<AnchorLine *> updateList(m_updateList);
    m_updateList.clear();
    Q_FOREACH (AnchorLine *line, updateList)
        line->anchorTo(nullptr);
}

void AnchorLine::setOffset(int val)
{
    if (m_offset == val)
        return;

    m_offset = val;

    this->updateList();
}

void AnchorLine::setOffsetDirection(int dir)
{
    if (m_edge != Horizontal && m_edge != Vertical)
        return;

    const int newdir = (dir < 0) ? -1 : 1;
    if (m_offsetDirection == newdir)
        return;

    m_offsetDirection = newdir;

    this->updateList();
}

AnchorLine *AnchorLine::anchorTo(AnchorLine *line)
{
    if (m_edge == Horizontal || m_edge == Vertical)
        return this;

    if (line == m_anchoredTo)
        return this;

    if (line != nullptr && this->layout() == line->layout())
        return this;

    if (m_anchoredTo != nullptr) {
        m_anchoredTo->removeFromUpdateList(this);
        m_anchoredTo = nullptr;
    }

    if (line == nullptr)
        return this;

    if (this->isVerticalLine() && !line->isVerticalLine())
        return this;

    // paranoia check
    if (this->isHorizontalLine() && !line->isHorizontalLine())
        return this;

    m_anchoredTo = line;
    m_anchoredTo->addToUpdateList(this);
    return this;
}

void AnchorLine::addToUpdateList(AnchorLine *line)
{
    if (line == nullptr || m_updateList.contains(line))
        return;

    m_updateList.append(line);
    m_layout->update();
}

void AnchorLine::removeFromUpdateList(AnchorLine *line)
{
    if (line == nullptr || !m_updateList.contains(line))
        return;

    m_updateList.removeOne(line);
    m_layout->update();
}

void AnchorLine::update()
{
    auto updateGeometry = [=]() {
        if (m_anchoredTo == nullptr)
            return;

        Relationship rel = relationship(this, m_anchoredTo);
        if (rel == NoRelationship)
            return;

        QWidget *w = this->widget();
        QRect geo = w->geometry();

        const QLine anchorLine = (rel == SiblingRelationship)
                ? m_anchoredTo->line(GeometryLine)
                : m_anchoredTo->line(RectLine);
        switch (m_edge) {
        case LeftEdge: {
            const int left = anchorLine.p1().x() + m_offsetDirection * m_offset;
            if (m_layout->right()->anchoredTo() == nullptr)
                geo.moveLeft(left);
            else
                geo.setLeft(left);
        } break;
        case TopEdge: {
            const int top = anchorLine.p1().y() + m_offsetDirection * m_offset;
            if (m_layout->bottom()->anchoredTo() == nullptr)
                geo.moveTop(top);
            else
                geo.setTop(top);
        } break;
        case RightEdge: {
            const int right =
                    anchorLine.p1().x() + m_offsetDirection * m_offset;
            if (m_layout->left()->anchoredTo() == nullptr)
                geo.moveRight(right);
            else
                geo.setRight(right);
        } break;
        case BottomEdge: {
            const int bottom =
                    anchorLine.p1().y() + m_offsetDirection * m_offset;
            if (m_layout->top()->anchoredTo() == nullptr)
                geo.moveBottom(bottom);
            else
                geo.setBottom(bottom);
        } break;
        case HCenter: {
            const int x = anchorLine.p1().x() + m_offsetDirection * m_offset;
            geo.moveCenter(QPoint(x, geo.center().y()));
        } break;
        case VCenter: {
            const int y = anchorLine.p1().y() + m_offsetDirection * m_offset;
            geo.moveCenter(QPoint(geo.center().x(), y));
        } break;
        default:
            break;
        }

        w->setGeometry(geo);
    };

    updateGeometry();
    this->updateList();
}

void AnchorLine::updateList()
{
    Q_FOREACH (AnchorLine *line, m_updateList)
        line->update();
}

QLine AnchorLine::line(LineMode mode) const
{
    const QRect rect = (mode == GeometryLine) ? m_layout->widget()->geometry()
                                              : m_layout->widget()->rect();
    switch (m_edge) {
    case LeftEdge:
        return QLine(rect.topLeft(), rect.bottomLeft());
    case TopEdge:
        return QLine(rect.topLeft(), rect.topRight());
    case RightEdge:
        return QLine(rect.topRight(), rect.bottomRight());
    case BottomEdge:
        return QLine(rect.bottomLeft(), rect.bottomRight());
    case HCenter:
        return QLine(rect.center().x(), rect.top(), rect.center().x(),
                     rect.bottom());
    case VCenter:
        return QLine(rect.left(), rect.center().y(), rect.right(),
                     rect.center().y());
    case Horizontal: {
        const QRectF rectf = rect;
        const qreal y = rectf.top() + rectf.height() * m_percent;
        return QLineF(rectf.left(), y, rectf.right(), y).toLine();
    };
    case Vertical: {
        const QRectF rectf = rect;
        const qreal x = rectf.left() + rectf.width() * m_percent;
        return QLineF(x, rectf.top(), x, rectf.bottom()).toLine();
    };
    }

    return QLine();
}

AnchorLine::Relationship AnchorLine::relationship(const AnchorLine *line1,
                                                  const AnchorLine *line2)
{
    if (line1 == nullptr || line2 == nullptr)
        return NoRelationship;

    QWidget *w1 = line1->widget();
    QWidget *w2 = line2->widget();

    if (w2 == w1->parentWidget())
        return ParentChildRelationship;

    if (w2->parentWidget() == w1->parentWidget())
        return SiblingRelationship;

    return NoRelationship;
}
