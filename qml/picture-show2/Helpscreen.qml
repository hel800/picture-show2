import QtQuick 2.0
import QtGraphicalEffects 1.0

Item {
    id: root
    width: 640
    height: 480

    function show_and_hide() {        
        if (_supervisor.blendingCount() > 1 || _supervisor.overlayTransitionCount() > 1 || !_supervisor.showLoaded()) {
            help_screen_smoother.visible = false;
            background.visible = false;
        }
        else if (help_screen.opacity === 0.0 ||  help_screen_smoother.visible) {
            help_screen_smoother.visible = true;
            background.visible = true;
        }

        if (help_screen.opacity == 0.0) {
            help_screen_smoother.opacity = 0.01;
            fade_help.start()
        }
        else {
            fade_help_out.start();
        }
    }

    function get_opacity() {
        return help_screen.opacity
    }

    function update(lang) {
        if (_settings_dialog.getLanguageForQml() === "de") {
            if (_settings_dialog.getMouseControlForQml())
                help_screen_commands.source = "qrc:///img/commands_mouse_de.png"
            else
                help_screen_commands.source = "qrc:///img/commands_keyboard_de.png"

            help_screen_title_text.text = "Kurzhilfe"
            help_screen_info_text1.text = "Dieses Programm unterliegt der GPL v3 Lizenz. Es kann frei verwendet und verändert werden, muss allerdings stets unter GPL Lizenz veröffentlicht werden. Es verwendet das Qt Framework (version 5). \n\n2012 by Sebastian Schäfer / web: http://code.google.com/p/picture-show/"

        }
        else {
            if (_settings_dialog.getMouseControlForQml())
                help_screen_commands.source = "qrc:///img/commands_mouse_en.png"
            else
                help_screen_commands.source = "qrc:///img/commands_keyboard_en.png"

            help_screen_title_text.text = "Quick Help"
            help_screen_info_text1.text = "This application is published under the GPL v3 licence. It can be used and modified freely, but has to be remain published under the GPL at all times. The Qt Framework (version 5) is used. \n\n2012 by Sebastian Schäfer / web: http://code.google.com/p/picture-show/"
        }
    }

    Rectangle {
        id: background
        width: parent.width
        height: parent.height
        color: "#222222"
        opacity: 0.0
    }

    FastBlur {
        id: help_screen_smoother
        anchors.fill: parent
        source: image_slot1
        radius: 32
        visible: true
        opacity: 0.0
        transparentBorder: true
    }

    Rectangle {
        id: help_screen
        color: "black"
        opacity: 0.0
        width: root.width
        height: root.height
    }

    Text {
        id: help_screen_version_info
        font.pixelSize: parent.height / 70
        font.family: textFont.name
        color: "#FFFFFF"
        anchors.bottom: parent.bottom
        anchors.bottomMargin: parent.height / 70
        anchors.horizontalCenter: parent.horizontalCenter
        smooth: true;
        text: "version: " + _supervisor.getAppVersion()
        opacity: help_screen.opacity
    }

    Rectangle {
        id: help_screen_content
        color: "#00000000"
        opacity: 0.0
        width: root.width
        height: root.height

        Image {
            id: help_screen_logo
            width: help_screen.width > help_screen.height ? help_screen.height / 2.5 : help_screen.width / 2.5
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            anchors.topMargin: help_screen.height / 30
            fillMode: Image.PreserveAspectFit
            source: "qrc:///img/help_logo.png"
            smooth: true
        }

        Rectangle {
            color: "white"
            anchors.fill: help_screen_commands
            anchors.topMargin: -help_screen_commands.height / 10
            anchors.bottomMargin: -help_screen_commands.height / 4
            anchors.margins: -help_screen_commands.height / 30
            radius: 10
            opacity: 0.1
        }

        Text {
            id: help_screen_title_text
            font.pixelSize: help_screen_commands.height / 20
            font.family: titleFont.name
            color: "#FFFFFF"
            anchors.bottom: help_screen_commands.top
            anchors.bottomMargin: help_screen_commands.height / 40
            anchors.horizontalCenter: help_screen_commands.horizontalCenter
            smooth: true;
            text: "Kurzhilfe"
        }

        Image {
            id: help_screen_commands
            height: (help_screen.width - 120) > help_screen.height ? help_screen.height / 2.0 : (help_screen.width - 120) / 2.0;
            fillMode: Image.PreserveAspectFit
            anchors.centerIn: parent
            source: "qrc:///img/commands_keyboard_de.png"
            smooth: true
        }

        Item {
            width: help_screen_commands.width
            height: help_screen_commands.height / 5
//            anchors.fill: help_screen_commands
            anchors.top: help_screen_commands.bottom
            anchors.topMargin: help_screen_commands.height / 20
            anchors.left: help_screen_commands.left
//            anchors.topMargin: parent.height - parent.height / 10

            Image {
                id: help_screen_info_image
                width: parent.height / 2.5
                fillMode: Image.PreserveAspectFit
                anchors.top: parent.top
                anchors.left: parent.left
                source: "qrc:///img/message_info.png"
                smooth: true
            }

            Text {
                id: help_screen_info_text1
                font.pixelSize: parent.height / 7
                font.family: textFont.name
                color: "#FFFFFF"
                anchors.top: parent.top
                anchors.left: help_screen_info_image.right
                anchors.leftMargin: 20
                anchors.right: parent.right
                smooth: true;
                wrapMode: Text.Wrap
                text: "Dieses Programm unterliegt der GPL v3 Lizenz. Es kann frei verwendet und verändert werden, muss allerdings stets unter GPL Lizenz veröffentlicht werden. Es verwendet das Qt Framework (version %1). \n\n2012 by Sebastian Schäfer / web: http://code.google.com/p/picture-show/"
            }

        }
    }

    ParallelAnimation {
        id: fade_help
        running: false

        onRunningChanged: {
            if (!fade_help.running) {
                _supervisor.overlayTransitionFinished()
            }
        }

        ScriptAction { script:  { if (image_slot1.opacity == 1.0) help_screen_smoother.source = image_slot1
                                  else if (image_slot2.opacity == 1.0) help_screen_smoother.source = image_slot2
                                  else if (image_slot3.opacity == 1.0) help_screen_smoother.source = image_slot3
                                }
                     }

        ScriptAction { script: { help_screen_content.scale= 0.8 } }

        NumberAnimation { target: background; properties: "opacity"; to: 1.0; duration: 500; easing.type: Easing.InOutQuad }
        NumberAnimation { id: fade_help_anim1; target: help_screen; properties: "opacity"; to: 0.7; duration: 300; easing.type: Easing.InOutQuad }
        NumberAnimation { id: fade_help_anim2; target: help_screen_content; properties: "opacity"; to: 1.0; duration: 300; easing.type: Easing.InOutQuad }
        NumberAnimation { id: fade_help_anim3; target: help_screen_content; easing.overshoot: 3; properties: "scale"; to: 1.0; duration: 400; easing.type: Easing.InOutBack }
        NumberAnimation { target: help_screen_smoother; properties: "radius"; to: 42; duration: 300; easing.type: Easing.OutQuint }
        NumberAnimation { target: help_screen_smoother; properties: "opacity"; to: 1.0; duration: 400; easing.type: Easing.InOutQuad }
    }

    ParallelAnimation {
        id: fade_help_out
        running: false

        onRunningChanged: {
            if (!fade_help_out.running) {
                _supervisor.overlayTransitionFinished()
            }
        }
        NumberAnimation { target: background; properties: "opacity"; to: 0.0; duration: 250; easing.type: Easing.InOutQuad }
        NumberAnimation { id: fade_help_out_anim1; target: help_screen; properties: "opacity"; to: 0.0; duration: 300; easing.type: Easing.InOutQuad }
        NumberAnimation { id: fade_help_out_anim2; target: help_screen_content; properties: "opacity"; to: 0.0; duration: 300; easing.type: Easing.InOutQuad }
        NumberAnimation { target: help_screen_smoother; properties: "radius"; to: 0; duration: 250; easing.type: Easing.InQuint }
        NumberAnimation { target: help_screen_smoother; properties: "opacity"; to: 0.0; duration: 250; easing.type: Easing.InOutQuad }

    }
}