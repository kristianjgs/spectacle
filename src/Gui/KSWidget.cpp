/*
 *  Copyright (C) 2015 Boudhayan Gupta <me@BaloneyGeek.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 */

#include "KSWidget.h"

KSWidget::KSWidget(QWidget *parent) :
    QWidget(parent)
{
    QFont tempFont;

    // we'll init the widget that holds the image first

    mImageWidget = new KSImageWidget(this);
    mImageWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    connect(mImageWidget, &KSImageWidget::dragInitiated, this, &KSWidget::dragInitiated);

    // the capture mode options first

    mCaptureModeLabel = new QLabel(this);
    mCaptureModeLabel->setText(i18n("Capture Mode"));
    tempFont = mCaptureModeLabel->font();
    tempFont.setPointSize(12);
    tempFont.setBold(true);
    mCaptureModeLabel->setFont(tempFont);

    mCaptureArea = new QComboBox(this);
    mCaptureArea->insertItem(0, i18n("Full Screen (All Monitors)"), ImageGrabber::FullScreen);
    mCaptureArea->insertItem(1, i18n("Current Screen"), ImageGrabber::CurrentScreen);
    mCaptureArea->insertItem(2, i18n("Active Window"), ImageGrabber::ActiveWindow);
    mCaptureArea->insertItem(3, i18n("Rectangular Region"), ImageGrabber::RectangularRegion);
    mCaptureArea->setMinimumWidth(200);
    connect(mCaptureArea, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &KSWidget::captureModeManage);

    mDelayMsec = new QDoubleSpinBox(this);
    mDelayMsec->setDecimals(1);
    mDelayMsec->setSingleStep(0.1);
    mDelayMsec->setMinimum(0.0);
    mDelayMsec->setMaximum(999.9);
    mDelayMsec->setSuffix(i18n(" seconds"));

    mCaptureOnClick = new QCheckBox(this);
    mCaptureOnClick->setText(i18n("On Click"));
    mCaptureOnClick->setToolTip(i18n("Wait for a mouse click before capturing the screenshot image"));
    connect(mCaptureOnClick, &QCheckBox::stateChanged, this, &KSWidget::onClickStateChanged);
    connect(mCaptureOnClick, &QCheckBox::stateChanged, this, &KSWidget::checkboxStatesChangedHandler);

    mDelayLayout = new QHBoxLayout;
    mDelayLayout->addWidget(mDelayMsec);
    mDelayLayout->addWidget(mCaptureOnClick);

    mCaptureModeForm = new QFormLayout;
    mCaptureModeForm->addRow(i18n("Capture Area"), mCaptureArea);
    mCaptureModeForm->addRow(i18n("Capture Delay"), mDelayLayout);
    mCaptureModeForm->setContentsMargins(24, 0, 0, 0);

    // the capture options (mouse pointer, window decorations)

    mCaptureOptionsLabel = new QLabel(this);
    mCaptureOptionsLabel->setText(i18n("Capture Options"));
    tempFont = mCaptureOptionsLabel->font();
    tempFont.setPointSize(12);
    tempFont.setBold(true);
    mCaptureOptionsLabel->setFont(tempFont);

    mMousePointer = new QCheckBox(this);
    mMousePointer->setText(i18n("Include mouse pointer"));
    mMousePointer->setToolTip(i18n("Show the mouse cursor in the screeenshot image"));
    connect(mMousePointer, &QCheckBox::stateChanged, this, &KSWidget::checkboxStatesChangedHandler);

    mWindowDecorations = new QCheckBox(this);
    mWindowDecorations->setText(i18n("Include window titlebar and borders"));
    mWindowDecorations->setToolTip(i18n("Show the window title bar, the minimize/maximize/close buttons, and the window border"));
    connect(mWindowDecorations, &QCheckBox::stateChanged, this, &KSWidget::checkboxStatesChangedHandler);

    mCaptureOptionsForm = new QVBoxLayout;
    mCaptureOptionsForm->addWidget(mMousePointer);
    mCaptureOptionsForm->addWidget(mWindowDecorations);
    mCaptureOptionsForm->setSpacing(16);
    mCaptureOptionsForm->setContentsMargins(24, 0, 0, 0);

    // the take new screenshot button

    mTakeScreenshotButton = new QPushButton(this);
    mTakeScreenshotButton->setText(i18n("Take New Screenshot"));
    mTakeScreenshotButton->setIcon(QIcon::fromTheme("ksnapshot"));
    mTakeScreenshotButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    mTakeScreenshotButton->setFocus();
    connect(mTakeScreenshotButton, &QPushButton::clicked, this, &KSWidget::newScreenshotClicked);

    // finally, finish up the layouts

    mRightLayout = new QVBoxLayout;
    mRightLayout->addWidget(mCaptureModeLabel);
    mRightLayout->addLayout(mCaptureModeForm);
    mRightLayout->addWidget(mCaptureOptionsLabel);
    mRightLayout->addLayout(mCaptureOptionsForm);
    mRightLayout->addSpacing(10);
    mRightLayout->addWidget(mTakeScreenshotButton, 1, Qt::AlignHCenter);
    mRightLayout->setContentsMargins(20, 0, 0, 10);

    mMainLayout = new QGridLayout(this);
    mMainLayout->addWidget(mImageWidget, 0, 0, 1, 1);
    mMainLayout->addLayout(mRightLayout, 0, 1, 1, 1);
    mMainLayout->setColumnMinimumWidth(0, 400);
    mMainLayout->setColumnMinimumWidth(1, 400);
}

// public slots

void KSWidget::setScreenshotPixmap(const QPixmap &pixmap)
{
    mImageWidget->setScreenshot(pixmap);
}

void KSWidget::setCheckboxStates(bool capturePointer, bool captureDecorations, bool captureOnClick)
{
    mMousePointer->setChecked(capturePointer);
    mWindowDecorations->setChecked(captureDecorations);
    mCaptureOnClick->setChecked(captureOnClick);
}

void KSWidget::setCaptureModeIndex(int index)
{
    mCaptureArea->setCurrentIndex(index);
}

void KSWidget::disableOnClick()
{
    mCaptureOnClick->setEnabled(false);
    mDelayMsec->setEnabled(true);
}

// private slots

void KSWidget::newScreenshotClicked()
{
    int delay = mCaptureOnClick->isChecked() ? -1 : (mDelayMsec->value() * 1000);
    ImageGrabber::GrabMode mode = static_cast<ImageGrabber::GrabMode>(mCaptureArea->currentData().toInt());
    emit newScreenshotRequest(mode, delay, mMousePointer->isChecked(), mWindowDecorations->isChecked());
}

void KSWidget::checkboxStatesChangedHandler(int state)
{
    Q_UNUSED(state);
    emit checkboxStatesChanged(mMousePointer->isChecked(), mWindowDecorations->isChecked(), mCaptureOnClick->isChecked());
}

void KSWidget::onClickStateChanged(int state)
{
    if (state == Qt::Checked) {
        mDelayMsec->setEnabled(false);
    } else if (state == Qt::Unchecked) {
        mDelayMsec->setEnabled(true);
    }
}

void KSWidget::captureModeManage(int index)
{
    emit captureModeChanged(index);

    ImageGrabber::GrabMode mode = static_cast<ImageGrabber::GrabMode>(mCaptureArea->itemData(index).toInt());
    switch (mode) {
    case ImageGrabber::ActiveWindow:
        mWindowDecorations->setEnabled(true);
        break;
    default:
        mWindowDecorations->setEnabled(false);
    }
}
