import QtQuick 2.0
import QtGraphicalEffects 1.0

Item {
    width: 640
    height: 480

    FastBlur {
        id: info_screen_smoother
        width: parent.width + parent.width / 10
        height: parent.width > parent.height ? parent.height / 7 : parent.width / 7;
        anchors.centerIn: info_screen;
        anchors.verticalCenterOffset: height / 6;
        source: info_screen
        radius: 48
        visible: true
        opacity: 0.0
        transparentBorder: true
    }

    Rectangle {
        id: info_screen
        color: "black"
        opacity: 0.0
        width: parent.width
        height: parent.width > parent.height ? parent.height / 7 : parent.width / 7;
        anchors.bottom: parent.bottom
    }

    Rectangle {
        id: info_screen_content
        color: "#00000000"
        opacity: 0.0
        width: parent.width
        height: parent.width > parent.height ? parent.height / 7 : parent.width / 7;
        anchors.bottom: parent.bottom

        Image {
            id: info_screen_info_image
            height: parent.height
            fillMode: Image.PreserveAspectFit
            anchors.bottom: parent.bottom
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottomMargin: height / 12
//            anchors.leftMargin: height / 4
            source: "qrc:///img/message_info.png"
            smooth: true
            opacity: 0.6
        }
    }

    function show_hide_infobox() {
        if (info_screen_content.opacity === 0.0) {
            fade_info.start()
        }
        else if (info_screen_content.opacity === 1.0) {
            fade_info_out.start()
        }
        else if (fade_info_out.running) {
            fade_info_out.stop()
            fade_info.start()
        }
        else {
            fade_info.stop()
            fade_info_out.start()
        }
    }

    ParallelAnimation {
        id: fade_info
        running: false

        onRunningChanged: {
            if (!fade_info.running) {
                _supervisor.overlayTransitionFinished()
            }
        }

        NumberAnimation { target: info_screen_content; properties: "opacity"; to: 1.0; duration: 500; easing.type: Easing.InOutQuad }
        NumberAnimation { target: info_screen_smoother; properties: "opacity"; to: 0.85; duration: 500; easing.type: Easing.InOutQuad }
    }

    ParallelAnimation {
        id: fade_info_out
        running: false

        onRunningChanged: {
            if (!fade_info_out.running) {
                _supervisor.overlayTransitionFinished()
            }
        }

        NumberAnimation { target: info_screen_content; properties: "opacity"; to: 0.0; duration: 500; easing.type: Easing.InOutQuad }
        NumberAnimation { target: info_screen_smoother; properties: "opacity"; to: 0.0; duration: 500; easing.type: Easing.InOutQuad }

    }
}
