/* ------------------------------------------------------------------
Messagebox.qml is part of picture-show2.
.....................................................................

picture-show is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

picture-show2 is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
You should have received a copy of the GNU General Public License
along with picture-show. If not, see <http://www.gnu.org/licenses/>.

......................................................................

author: Sebastian Schäfer
February 2013

--------------------------------------------------------------------*/

import QtQuick 6.0
import Qt5Compat.GraphicalEffects

Item {
    id: message_screen_root
    width: 640
    height: 480

    Keys.onUpPressed: message_box.up_pressed()
    Keys.onDownPressed: message_box.down_pressed()
    Keys.onEnterPressed: message_box.enter_pressed()
    Keys.onReturnPressed: message_box.enter_pressed()

    Rectangle {
        id: background
        width: parent.width
        height: parent.height
        color: "#222222"
        opacity: 0.0
    }

    FastBlur {
        id: message_screen_smoother
        anchors.fill: parent
        source: image_slot1
        radius: 32
        visible: true
        opacity: 0.0
        transparentBorder: true
    }

    Rectangle {
        id: message_screen
        color: "black"
        opacity: 0.0
        width: parent.width
        height: parent.width * 0.65 > parent.height ? parent.height / 6 : (parent.width * 0.65)  / 6;
        anchors.verticalCenter: parent.verticalCenter
    }

    Rectangle {
        id: message_screen_content
        color: "#00000000"
        opacity: 0.0
        width: parent.width
        height: parent.height

        Image {
            id: message_screen_image
            width: message_screen.height * 1.5
            height: message_screen.height * 1.5
            source: "qrc:///img/message_error.png"
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: parent.width / 12
            smooth: true;
            scale: 0.8
        }

        Text {
            id: message_screen_title
            font.pixelSize: message_screen.height / 5
            font.family: titleFont.name
            elide: Text.ElideRight
            color: "#FFFFFF"
            width: button_first.visible ? parent.width - (parent.width / 10 + 1.3 * message_screen_image.width + (parent.width * 0.06 + button_first.width)) : parent.width - (parent.width / 10 + 1.3 * message_screen_image.width)

            anchors.verticalCenter: parent.verticalCenter
            anchors.verticalCenterOffset: -message_screen.height / 5
            anchors.left: parent.left
            anchors.leftMargin: parent.width / 12 + 1.3 * message_screen_image.width

            smooth: true;
            text: ""
            opacity: 0.8
        }

        Text {
            id: message_screen_title2
            font.pixelSize: message_screen.height / 4
            font.family: titleFont.name
            elide: Text.ElideRight
            color: "#FFFFFF"
            width: button_first.visible ? parent.width - (parent.width / 10 + 1.3 * message_screen_image.width + (parent.width * 0.06 + button_first.width)) : parent.width - (parent.width / 10 + 1.3 * message_screen_image.width)

            anchors.verticalCenter: parent.verticalCenter
            anchors.verticalCenterOffset: 0
            anchors.left: parent.left
            anchors.leftMargin: parent.width / 12 + 1.3 * message_screen_image.width

            smooth: true;
            text: ""
            opacity: 0.8
        }

        Image {
            id: message_screen_info_image
            width: message_screen.height / 5
            height: message_screen.height / 5
            source: "qrc:///img/message_info.png"
            anchors.verticalCenter: parent.verticalCenter
            anchors.verticalCenterOffset: message_screen.height / 6
            anchors.left: parent.left
            anchors.leftMargin: parent.width / 12 + 1.3 * message_screen_image.width
            smooth: true
            visible: false
        }

        Text {
            id: message_screen_text
            font.pixelSize: text.length > 50 ? message_screen.height / 8 : message_screen.height / 6
            font.family: textFont.name
            elide: Text.ElideLeft
            color: "#FFFFFF"
            wrapMode: Text.Wrap
            maximumLineCount: 3
            width: button_first.visible ? parent.width - (parent.width / 10 + 1.3 * message_screen_image.width + (parent.width * 0.06 + button_first.width)) : parent.width - (parent.width / 10 + 1.3 * message_screen_image.width)

            anchors.verticalCenter: parent.verticalCenter
            anchors.verticalCenterOffset: message_screen.height / 6
            anchors.left: parent.left
            anchors.leftMargin: message_screen_info_image.visible ? parent.width / 12 + 1.3 * message_screen_image.width + 1.3 * message_screen_info_image.width : parent.width / 12 + 1.3 * message_screen_image.width

            smooth: true;
            opacity: 0.8
            text: ""
        }

//        Rectangle {
//            id: image_jumpto_wait_rect
//            color: "black"
//            height: message_screen.height * 1.3
//            width: height * 1.5
//            anchors.verticalCenter: parent.verticalCenter
//            anchors.horizontalCenter: parent.horizontalCenter
//            anchors.horizontalCenterOffset: parent.width * 0.3
//            opacity: 0.0
//            radius: 10.0
//            smooth: true
//        }

        Image {
            id: image_jumpto_wait
            width: message_screen.height / 2
            fillMode: Image.PreserveAspectFit
            source: "qrc:///img/wait.png"

            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: image_jumpto_rect.horizontalCenter

            opacity: 0.0
        }

        Image {
            id: image_jumpto_preview_error
            height: message_screen.height / 3
            fillMode: Image.PreserveAspectFit
            source: "qrc:///img/error_small.png"

            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: image_jumpto_rect.horizontalCenter

            opacity: 0.0

            Behavior on opacity {
               NumberAnimation { duration: 300 }
            }
        }

        Rectangle {
            id: image_jumpto_rect
            color: "black"
            height: _settings_dialog.getScaleTypeQml() === 2 ? image_jumpto.height + (message_screen.height * 0.1755) : image_jumpto.paintedHeight + (message_screen.height * 0.1755)
            width: _settings_dialog.getScaleTypeQml() === 2 ? image_jumpto.width + (message_screen.height * 0.1755) : image_jumpto.paintedWidth + (message_screen.height * 0.1755)
            anchors.centerIn: image_jumpto
            opacity: 0.0
            radius: 10.0
            scale: 0.5
            smooth: true
        }

        Image {
            id: image_jumpto
            height: message_screen.height * 1.35
            width: _settings_dialog.getScaleTypeQml() === 2 ? height * (message_screen_root.width / message_screen_root.height) : height * 1.7
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.horizontalCenterOffset: parent.width * 0.3
            fillMode: _settings_dialog.getScaleTypeQml() === 2 ? Image.PreserveAspectCrop : Image.PreserveAspectFit
//            fillMode: Image.PreserveAspectFit
            smooth: true
            opacity: 0.0
            scale: 0.5

            onStatusChanged: if (image_jumpto.status === Image.Ready) _supervisor.imageLoadingFinished(source)
        }

        Rectangle {
            id: button_first
            focus: false
            visible: false

            color: focus ? "#666666" : "#333333"
            border.color: focus ? "#cccccc" : "#00000000"
            border.width: parent.width * 0.002
            opacity: focus ? 0.8 : 0.6
            radius: parent.width * 0.65 > parent.height ? parent.height / 24 : (parent.width * 0.65)  / 24;

            width: parent.width * 0.2
            height: parent.width * 0.65 > parent.height ? parent.height / 12 : (parent.width * 0.65)  / 12;

            anchors.right: parent.right
            anchors.rightMargin: parent.width * 0.06
            anchors.verticalCenter: parent.verticalCenter
            anchors.verticalCenterOffset: -height * 0.7

            Behavior on color {
               ColorAnimation { duration: 500 }
            }

            Text {
                id: button_first_text
                font.pixelSize: parent.height / 2;
                font.family: textFont.name
                elide: Text.ElideRight
                color: parent.focus ? "#EEEEEE" : "#AAAAAA"

                anchors.centerIn: parent

                text: ""
            }
        }

        Rectangle {
            id: button_second
            focus: false
            visible: false

            color: focus ? "#666666" : "#333333"
            border.color: focus ? "#cccccc" : "#00000000"
            border.width: parent.width * 0.002
            smooth: true

            opacity: focus ? 0.8 : 0.6
            radius: parent.width * 0.65 > parent.height ? parent.height / 24 : (parent.width * 0.65)  / 24;

            width: parent.width * 0.2
            height: parent.width * 0.65 > parent.height ? parent.height / 12 : (parent.width * 0.65)  / 12;

            anchors.right: parent.right
            anchors.rightMargin: parent.width * 0.06
            anchors.verticalCenter: parent.verticalCenter
            anchors.verticalCenterOffset: height * 0.7

            Behavior on color {
               ColorAnimation { duration: 500 }
            }

//            MouseArea {
//                id: mouseArea2
//                anchors.fill: parent
//                hoverEnabled: true

//                acceptedButtons: Qt.AllButtons

//                onPressed: {
//                    console.log("entered")

//                    if (mouse.button == Qt.LeftButton)
//                        _supervisor.answerOfQuestion(1)
//                }
//            }

            Text {
                id: button_second_text
                font.pixelSize: parent.height / 2;
                font.family: textFont.name
                elide: Text.ElideRight
                color: parent.focus ? "#EEEEEE" : "#AAAAAA"

                anchors.centerIn: parent

                text: ""
            }
        }
    }

    function set_background_color(color) {
        background.color = color;
    }

    function hide_message() {
        if (image_jumpto.opacity !== 0.0) {
            fade_jumpto_preview.stop()
            fade_jumpto_preview_out.start()
        }

        if (image_jumpto_wait.opacity !== 0.0) {
            fade_jumpto_preview_wait1.stop();
            fade_jumpto_preview_wait2.start();
        }

        image_jumpto_preview_error.opacity = 0.0

        if (message_screen.opacity === 0.0) {
            _supervisor.overlayTransitionFinished()
        }
        else if (message_screen.opacity === 0.7) {
            fade_message_out.start()
        }
        else if (fade_message.running) {
            fade_message.stop()
            fade_message_out.start()
        }
    }

    function show_message(image, title, text, info, numButtons, text_b1, text_b2) {
        if (image_jumpto.opacity !== 0.0 && message_screen_title.text !== title) {
            fade_jumpto_preview.stop()
            fade_jumpto_preview_out.start()
        }

        if (image_jumpto_wait.opacity !== 0.0) {
            fade_jumpto_preview_wait1.stop();
            fade_jumpto_preview_wait2.start();
        }

        image_jumpto_preview_error.opacity = 0.0

        if (title !== "" && text !== "") {
            message_screen_title2.text = ""
            message_screen_title.text = title
            message_screen_text.text = text
            message_screen_image.source = image

        }
        else if (title !== "" && text === "") {
            message_screen_title.text = ""
            message_screen_title2.text = title
            message_screen_text.text = text
            message_screen_image.source = image
        }

        // question dialog?
        if (numButtons > 0) {
            button_first.visible = true
            button_second.visible = true
            button_first.focus = true
            button_first_text.text = text_b1
            button_second_text.text = text_b2
        }
        else {
            button_first.visible = false
            button_second.visible = false
        }

        if (hp_screen.get_opacity() > 0.0 || _supervisor.blendingCount() > 0 || _supervisor.overlayTransitionCount() > 1 || !_supervisor.showLoaded()) {
            message_screen_smoother.visible = false;
            background.visible = false;
        }
        else if (message_screen.opacity > 0.0) {
            message_screen_smoother.visible = message_screen_smoother.visible;
            background.visible = background.visible;
        }
        else {
            message_screen_smoother.visible = true;
            background.visible = true;
        }

        if (info)
            message_screen_info_image.visible = true
        else
            message_screen_info_image.visible = false

        if (message_screen.opacity === 0.0) {
            message_screen_smoother.opacity = 0.01;
            fade_message.start()
        }
        else if (message_screen.opacity === 0.7) {
            _supervisor.overlayTransitionFinished()
        }
        else if (fade_message_out.running) {
            fade_message_out.stop()
            fade_message.start()
        }
        else if (fade_message.running) {
            _supervisor.overlayTransitionFinished()
        }
    }

    function update_preview_dimensions() {
        image_jumpto.width = _settings_dialog.getScaleTypeQml() === 2 ? image_jumpto.height * (message_screen_root.width / message_screen_root.height) : image_jumpto.height * 1.7
        image_jumpto.fillMode = _settings_dialog.getScaleTypeQml() === 2 ? Image.PreserveAspectCrop : Image.PreserveAspectFit
    }

//    function hide_instantly() {
//        background.opacity = 0.0
//        message_screen.opacity = 0.0
//        message_screen_content.opacity = 0.0
//        message_screen_image.scale = 0.8
//        message_screen_smoother.radius = 0
//        message_screen_smoother.opacity = 0.0
//    }

    function load_jumpto_image(image) {
        image_jumpto_preview_error.opacity = 0.0

        if ((image_jumpto.source == "image://pictures/" + image) && (image_jumpto.status === Image.Ready)) {
            _supervisor.imageLoadingFinished(image_jumpto.source)
            return
        }

        if (fade_jumpto_preview_wait2.running)
            fade_jumpto_preview_wait2.stop();
        fade_jumpto_preview_wait1.start();

        image_jumpto.source = "image://pictures/" + image
    }

    function blend_jumpto() {
        image_jumpto.width = _settings_dialog.getScaleTypeQml() === 2 ? image_jumpto.height * (message_screen_root.width / message_screen_root.height) : image_jumpto.height * 1.7
        image_jumpto.fillMode = _settings_dialog.getScaleTypeQml() === 2 ? Image.PreserveAspectCrop : Image.PreserveAspectFit

//        if (_settings_dialog.getScaleTypeQml() === 2)
//        {
//            image_jumpto_rect.height = image_jumpto.height + (message_screen.height * 0.1755)
//            image_jumpto_rect.width = image_jumpto.width + (message_screen.height * 0.1755)
//        }
//        else
//        {
//            if ((image_jumpto.sourceSize.width / image_jumpto.sourceSize.height) > 1.7) {
//                image_jumpto_rect.height = (image_jumpto.width / (image_jumpto.sourceSize.width / image_jumpto.sourceSize.height)) + (message_screen.height * 0.1755)
//                image_jumpto_rect.width = image_jumpto.width + (message_screen.height * 0.1755)
//            }
//            else {
//                image_jumpto_rect.height = image_jumpto.height + (message_screen.height * 0.1755)
//                image_jumpto_rect.width = (image_jumpto.height * (image_jumpto.sourceSize.width / image_jumpto.sourceSize.height)) + (message_screen.height * 0.1755)
//            }
//        }

        if (fade_jumpto_preview_wait1.running)
            fade_jumpto_preview_wait1.stop();

        if (!fade_jumpto_preview_wait2.running)
            fade_jumpto_preview_wait2.start();

        fade_jumpto_preview_out.stop()
        fade_jumpto_preview.start()
    }

    function blend_jumpto_preview_error() {
        if (fade_jumpto_preview_wait1.running)
            fade_jumpto_preview_wait1.stop();

        if (!fade_jumpto_preview_wait2.running)
            fade_jumpto_preview_wait2.start();

        image_jumpto_preview_error.opacity = 0.4
    }

    function get_jumpto_source() {
        return image_jumpto.source
    }

    function up_pressed() {
        if (!button_first.focus) {
            button_first.focus = true
        }
    }

    function down_pressed() {
        if (!button_second.focus) {
            button_second.focus = true
        }
    }

    function enter_pressed() {
        if (button_first.focus)
            _supervisor.answerOfQuestion(0)
        else
            _supervisor.answerOfQuestion(1)
    }

    ParallelAnimation {
        id: fade_message
        running: false

        onRunningChanged: {
            if (!fade_message.running) {
                _supervisor.overlayTransitionFinished()
            }
        }

        ScriptAction { script:  { //message_screen_smoother.radius = 0;
                                  if (image_slot1.opacity == 1.0) message_screen_smoother.source = image_slot1
                                  else if (image_slot2.opacity == 1.0) message_screen_smoother.source = image_slot2
                                  else if (image_slot3.opacity == 1.0) message_screen_smoother.source = image_slot3
                                }
                     }

        NumberAnimation { target: background; properties: "opacity"; to: 1.0; duration: 500; easing.type: Easing.InOutQuad }
        NumberAnimation { target: message_screen; properties: "opacity"; to: 0.7; duration: 500; easing.type: Easing.InOutQuad }
        NumberAnimation { target: message_screen_content; properties: "opacity"; to: 1.0; duration: 500; easing.type: Easing.InOutQuad }
        NumberAnimation { target: message_screen_image; easing.overshoot: 4; properties: "scale"; to: 1.0; duration: 500; easing.type: Easing.InOutBack }
        NumberAnimation { target: message_screen_smoother; properties: "radius"; to: 42; duration: 300; easing.type: Easing.OutQuint }
        NumberAnimation { target: message_screen_smoother; properties: "opacity"; to: 1.0; duration: 500; easing.type: Easing.InOutQuad }

    }

    ParallelAnimation {
        id: fade_message_out
        running: false

        onRunningChanged: {
            if (!fade_message_out.running) {
                _supervisor.overlayTransitionFinished()
            }
        }

        NumberAnimation { target: background; properties: "opacity"; to: 0.0; duration: 250; easing.type: Easing.InOutQuad }
        NumberAnimation { target: message_screen; properties: "opacity"; to: 0.0; duration: 250; easing.type: Easing.InOutQuad }
        NumberAnimation { target: message_screen_content; property: "opacity"; to: 0.0; duration: 250 }
        NumberAnimation { target: message_screen_image; properties: "scale"; to: 0.8; duration: 250; easing.type: Easing.InOutQuad }
        NumberAnimation { target: message_screen_smoother; properties: "radius"; to: 0; duration: 250; easing.type: Easing.InQuint }
        NumberAnimation { target: message_screen_smoother; properties: "opacity"; to: 0.0; duration: 250; easing.type: Easing.InOutQuad }
    }

    SequentialAnimation {
        id: fade_jumpto_preview
        running: false

        onRunningChanged: {
            if (!fade_jumpto_preview.running && image_jumpto.scale === 1.0) {
                _supervisor.jumtoPreviewReady()
            }
        }

        ParallelAnimation {
            NumberAnimation { target: image_jumpto; properties: "opacity"; to: 0.85; duration: 650; easing.type: Easing.InOutQuad }
            NumberAnimation { target: image_jumpto; properties: "scale"; to: 1.0; duration: 650; easing.type: Easing.InOutBack }
            NumberAnimation { target: image_jumpto_rect; properties: "opacity"; to: 0.5; duration: 650; easing.type: Easing.InOutQuad }
            NumberAnimation { target: image_jumpto_rect; properties: "scale"; to: 1.0; duration: 650; easing.type: Easing.InOutBack }
        }
    }

    SequentialAnimation {
        id: fade_jumpto_preview_out
        running: false

        ParallelAnimation {
            NumberAnimation { target: image_jumpto; properties: "opacity"; to: 0.0; duration: 200; easing.type: Easing.InOutQuad }
            NumberAnimation { target: image_jumpto; properties: "scale"; to: 0.5; duration: 200; easing.type: Easing.InOutQuad }
            NumberAnimation { target: image_jumpto_rect; properties: "opacity"; to: 0.0; duration: 200; easing.type: Easing.InOutQuad }
            NumberAnimation { target: image_jumpto_rect; properties: "scale"; to: 0.5; duration: 200; easing.type: Easing.InOutQuad }
        }

//        ScriptAction {
//            script: {
//                console.log(image_jumpto.paintedWidth)
//                console.log(image_jumpto.paintedHeight)
//                console.log("")
//            }
//        }
    }

    ParallelAnimation {
        id: fade_jumpto_preview_wait1
        running: false

        ScriptAction { script:  { fade_jumpto_preview_wait3.start() } }

        NumberAnimation { target: image_jumpto_wait; properties: "opacity"; to: 0.6; duration: 300; easing.type: Easing.InOutQuad }
}

    SequentialAnimation {
        id: fade_jumpto_preview_wait2
        running: false

        NumberAnimation { target: image_jumpto_wait; properties: "opacity"; to: 0.0; duration: 200; easing.type: Easing.OutCubic }

        ScriptAction { script:  { fade_jumpto_preview_wait3.stop() } }
    }

    ParallelAnimation {
        id: fade_jumpto_preview_wait3
        running: false

        RotationAnimation { target: image_jumpto_wait; property: "rotation"; from: 0; to: 360; duration: 2500; loops: Animation.Infinite }
    }

}
