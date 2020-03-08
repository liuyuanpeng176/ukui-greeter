/* userentry.cpp
 * Copyright (C) 2018 Tianjin KYLIN Information Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301, USA.
**/
#include "userentry.h"
#include <QPainter>
#include <QBrush>
#include <QFile>
#include <QMouseEvent>
#include <QDebug>
#include <QGraphicsOpacityEffect>
#include <QLightDM/UsersModel>
#include "globalv.h"

int UserEntry::count = 0;
int UserEntry::selectedId = 0;

UserEntry::UserEntry(const QString &name, const QString &facePath, bool isLogin, QWidget *parent)
    : QWidget(parent),
      id(count++)
{
    initUI();

    setUserName(name);
    setFace(facePath);
    setLogin(isLogin);
}

UserEntry::UserEntry(QWidget *parent)
    : UserEntry("kylin", "", true, parent)
{
    setUserName("Kylin"+QString::number(id));
}

void UserEntry::initUI()
{
    if (objectName().isEmpty())
        setObjectName(QString::fromUtf8("Entry")+QString::number(id));
    m_faceLabel = new QLabel(this);
    m_faceLabel->setObjectName(QString::fromUtf8("faceLabel"));
    m_faceLabel->setScaledContents(true);
    m_faceLabel->installEventFilter(this);

    m_nameLabel = new QLabel(this);
    m_nameLabel->setObjectName(QString::fromUtf8("nameLabel"));
    m_nameLabel->setFont(QFont("Ubuntu", fontSize));

    m_loginLabel = new QLabel(this);
    m_loginLabel->setObjectName(QString::fromUtf8("isloginLabel"));
    m_loginLabel->setAlignment(Qt::AlignCenter);

}

void UserEntry::paintEvent(QPaintEvent *event)
{
    //绘制阴影边框

    return QWidget::paintEvent(event);
}

void UserEntry::resizeEvent(QResizeEvent *)
{
    setResize();
}

bool UserEntry::eventFilter(QObject *obj, QEvent *event)
{
    if(obj == m_faceLabel){
        if(event->type() == QEvent::MouseButtonPress){
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            if(mouseEvent->button() == Qt::LeftButton){
                Q_EMIT pressed();
                return true;
            }
        }
    }
    return QWidget::eventFilter(obj, event);
}

void UserEntry::onClicked()
{
    this->setSelected();
    emit clicked(index.row());
}

QPixmap UserEntry::PixmapToRound(const QPixmap &src, int radius)
{
    if (src.isNull()) {
        return QPixmap();
    }

    QPixmap pixmapa(src);
    QPixmap pixmap(radius*2,radius*2);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    QPainterPath path;
    path.addEllipse(0, 0, radius*2, radius*2);
    painter.setClipPath(path);
    painter.drawPixmap(0, 0, radius*2, radius*2, pixmapa);
    return pixmap;
}


void UserEntry::setFace(const QString &facePath)
{
    this->m_face = facePath;
    QFile faceFile(facePath);
    if(!faceFile.exists())
        this->m_face = ":/resource/default_face.png";

    if(id == selectedId){
        const QString SheetStyle = QString("border-radius: %1px;  border:2px   solid white;").arg(CENTER_IMG_WIDTH/2);
        m_faceLabel->setStyleSheet(SheetStyle);
        userface = scaledPixmap(CENTER_IMG_WIDTH, CENTER_IMG_WIDTH, m_face);
        userface =  PixmapToRound(userface, CENTER_IMG_WIDTH/2);
    }
    else{
        const QString SheetStyle = QString("border-radius: %1px;  border:2px   solid white;").arg(IMG_WIDTH/2);
        m_faceLabel->setStyleSheet(SheetStyle);
        userface = scaledPixmap(IMG_WIDTH, IMG_WIDTH, m_face);
        userface =  PixmapToRound(userface, IMG_WIDTH/2);
    }

    m_faceLabel->setAlignment(Qt::AlignCenter);
    m_faceLabel->setPixmap(userface);

}

const QString& UserEntry::userName()
{
    return this->m_name;
}

void UserEntry::setUserName(const QString &name)
{
    if(m_name != name)
        m_name = name;
    this->m_nameLabel->setText(m_name);
    /* 当用户名短的时候居中， 否则是居左显示前半部分 */
    QFont font("ubuntu", 24);
    QFontMetrics fm(font);
    int pixelsWide = fm.width(m_name);
    if(pixelsWide < m_nameLabel->width())
        m_nameLabel->setAlignment(Qt::AlignCenter);
    else{
        QFontMetrics fontWidth(font);
        QString str = fontWidth.elidedText(m_name,Qt::ElideRight,m_nameLabel->width());
        this->m_nameLabel->setText(str);
    }
}

void UserEntry::setLogin(bool isLogin)
{
    if(m_login != isLogin)
        m_login = isLogin;
    if(isLogin)
        m_loginLabel->show();
    else
        m_loginLabel->hide();
}

void UserEntry::setResize()
{
    QRect faceRect,nameRect,loginRect;
    if(id == selectedId)
    {
        const QString SheetStyle = QString("border-radius: %1px;  border:2px   solid white;").arg(CENTER_IMG_WIDTH/2);
        m_faceLabel->setStyleSheet(SheetStyle);
        faceRect.setRect(SHADOW_WIDTH, SHADOW_WIDTH, CENTER_FACE_WIDTH, CENTER_FACE_WIDTH);
        userface = scaledPixmap(CENTER_IMG_WIDTH, CENTER_IMG_WIDTH, m_face);
        userface =  PixmapToRound(userface, CENTER_IMG_WIDTH/2);
        m_faceLabel->setGeometry(faceRect);
        m_faceLabel->move((width() - m_faceLabel->width())/2,m_faceLabel->y());
        m_nameLabel->setFont(QFont("Ubuntu", 24));
        QGraphicsOpacityEffect *opacityEffect=new QGraphicsOpacityEffect;
        m_faceLabel->setGraphicsEffect(opacityEffect);
        opacityEffect->setOpacity(1);
    }
    else
    {
        const QString SheetStyle = QString("border-radius: %1px;  border:2px   solid white;").arg(IMG_WIDTH/2);
        m_faceLabel->setStyleSheet(SheetStyle);
        faceRect.setRect(SHADOW_WIDTH, SHADOW_WIDTH, FACE_WIDTH, FACE_WIDTH);
        userface = scaledPixmap(IMG_WIDTH, IMG_WIDTH, m_face);
        userface =  PixmapToRound(userface, IMG_WIDTH/2);
        m_faceLabel->setGeometry(faceRect);
        m_faceLabel->move((width() - m_faceLabel->width())/2,m_faceLabel->y()+30);
        m_nameLabel->setFont(QFont("Ubuntu", 18));
        QGraphicsOpacityEffect *opacityEffect=new QGraphicsOpacityEffect;
        m_faceLabel->setGraphicsEffect(opacityEffect);
        opacityEffect->setOpacity(0.6);
    }


    m_faceLabel->setPixmap(userface);

    m_nameLabel->adjustSize();
    m_nameLabel->move((width() - m_nameLabel->width())/2,m_faceLabel->y() + m_faceLabel->height());

    m_loginLabel->setPixmap(QPixmap(":/resource/is_logined.png"));
    m_loginLabel->resize(24,24);
    m_loginLabel->setGeometry(m_faceLabel->x(),m_faceLabel->y(),24,24);


}

void UserEntry::setSelected(bool selected)
{
    if(selected) {
        selectedId = this->id;
    }

    setResize();
}

bool UserEntry::selected()
{
    return id == selectedId;
}

void UserEntry::setUserIndex(const QPersistentModelIndex &index)
{
    Q_ASSERT(index.isValid());
    this->index = index;

    QString realName = index.data(QLightDM::UsersModel::RealNameRole).toString();
    QString name = index.data(QLightDM::UsersModel::NameRole).toString();
    QString facePath = index.data(QLightDM::UsersModel::ImagePathRole).toString();
    bool isLoggedIn = index.data(QLightDM::UsersModel::LoggedInRole).toBool();

    setUserName(realName.isEmpty() ? name : realName);
    setFace(facePath);
    setLogin(isLoggedIn);
}

QPersistentModelIndex UserEntry::userIndex()
{
    return index;
}