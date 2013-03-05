import QtQuick 2.0
import QtGraphicalEffects 1.0

Item {
    width: 640
    height: 480

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
        height: parent.width > parent.height ? parent.height / 6 : parent.width / 6;
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
            color: "#FFFFFF"
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
            color: "#FFFFFF"
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
            font.pixelSize: message_screen.height / 6
            font.family: textFont.name
            color: "#FFFFFF"
            wrapMode: Text.Wrap
            width: parent.width - (parent.width / 12 + 1.3 * message_screen_image.width)
            anchors.verticalCenter: parent.verticalCenter
            anchors.verticalCenterOffset: message_screen.height / 6
            anchors.left: parent.left
            anchors.leftMargin: message_screen_info_image.visible ? parent.width / 12 + 1.3 * message_screen_image.width + 1.3 * message_screen_info_image.width : parent.width / 12 + 1.3 * message_screen_image.width
            smooth: true;
            opacity: 0.8
            text: ""
        }
    }

    function show_hide_message(image, title, text, info) {
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

        // show
        if (title !== "" || text !== "") {
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
        // hide
        else {
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
}
