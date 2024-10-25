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

#ifndef ANCHORLAYOUT_H
#define ANCHORLAYOUT_H

#include <QBasicTimer>
#include <QObject>
#include <QWidget>

class AnchorLine;
class AnchorLayout : public QObject
{
    Q_OBJECT

public:
    static AnchorLayout *get(QWidget *widget);

    AnchorLayout(QWidget *widget);
    ~AnchorLayout();

    QWidget *widget() const { return m_widget; }

    AnchorLine *left();
    AnchorLine *top();
    AnchorLine *right();
    AnchorLine *bottom();
    AnchorLine *horizontalCenter();
    AnchorLine *verticalCenter();

    enum OffsetDirection {
        OD_Auto = -2,
        OD_Left = -1,
        OD_Right = 1,
        OD_Up = OD_Left,
        OD_Down = OD_Right
    };
    AnchorLine *customLine(Qt::Orientation orientation, qreal percent,
                           OffsetDirection offsetDirection = OD_Auto);

    void centerIn(AnchorLayout *other);
    AnchorLayout *fill(AnchorLayout *other);

    void setMargins(int margin);

    void update();

signals:
    void geometryChanged(const QRect &rect);

private:
    bool eventFilter(QObject *object, QEvent *event);
    void timerEvent(QTimerEvent *te);
    bool isAnchorAllowed(AnchorLayout *layout) const;
    bool isAnchorAllowed(AnchorLine *line) const;

private:
    friend class AnchorLine;
    QWidget *m_widget;
    int m_margins;

    AnchorLine *m_leftLine;
    AnchorLine *m_rightLine;
    AnchorLine *m_topLine;
    AnchorLine *m_bottomLine;
    AnchorLine *m_hcenterLine;
    AnchorLine *m_vcenterLine;
    AnchorLayout *m_centerIn;
    AnchorLayout *m_fill;
    QList<AnchorLine *> m_customLines;

    QBasicTimer m_updateTimer;
};

class AnchorLine : public QObject
{
    Q_OBJECT

public:
    enum Edge {
        LeftEdge,
        TopEdge,
        RightEdge,
        BottomEdge,
        HCenter,
        VCenter,
        Horizontal,
        Vertical
    };
    ~AnchorLine();

    Edge edge() const { return m_edge; }
    AnchorLayout *layout() const { return m_layout; }
    QWidget *widget() const { return m_layout->widget(); }

    bool isVerticalLine() const
    {
        return m_edge == LeftEdge || m_edge == RightEdge || m_edge == HCenter
                || m_edge == Vertical;
    }
    bool isHorizontalLine() const
    {
        return m_edge == TopEdge || m_edge == BottomEdge || m_edge == VCenter
                || m_edge == Horizontal;
    }

    void setMargin(int val) { this->setOffset(val); }
    int margin() const { return this->offset(); }

    void setOffset(int val);
    int offset() const { return m_offset; }

    void setMarginDirection(int dir);
    int marginDirection() const;

    void setOffsetDirection(int dir);
    int offsetDirection() const { return m_offsetDirection; }

    AnchorLine *anchorTo(AnchorLine *line);
    AnchorLine *anchoredTo() const { return m_anchoredTo; }

private:
    AnchorLine(AnchorLayout *layout, Edge edge, qreal percent = 0);
    void addToUpdateList(AnchorLine *line);
    void removeFromUpdateList(AnchorLine *line);
    void update();
    void updateList();

    enum LineMode { GeometryLine, RectLine };
    QLine line(LineMode mode = GeometryLine) const;

    enum Relationship {
        NoRelationship,
        SiblingRelationship,
        ParentChildRelationship
    };
    static Relationship relationship(const AnchorLine *line1,
                                     const AnchorLine *line2);

private:
    friend class AnchorLayout;
    AnchorLayout *m_layout;
    Edge m_edge;
    qreal m_percent;
    int m_offset;
    int m_offsetDirection;
    QList<AnchorLine *> m_updateList;
    AnchorLine *m_anchoredTo;
};

#endif // ANCHORLAYOUT_H
