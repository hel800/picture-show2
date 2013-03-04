import QtQuick 2.0
import QtGraphicalEffects 1.0

Item {
    id: info_screen_root
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
        height: parent.width > parent.height ? (parent.height / 7) - ((parent.height / 7) / 6) : (parent.width / 7) - ((parent.width / 7) / 6);
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
            scale: 0.65
        }

        Text {
            id: info_screen_imageNumber
            font.pixelSize: parent.height / 4
            font.family: titleFont.name
            color: "#FFFFFF"
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: parent.width / 30
            smooth: true;
            text: ""
            opacity: 0.9
        }

        Grid {
            id: info_screen_leftGrid_annotations
            height: parent.height - parent.height / 3
            width: parent.height
            anchors.verticalCenter: parent.verticalCenter
            anchors.verticalCenterOffset: height / 12
            anchors.left: info_screen_imageNumber.right
            anchors.leftMargin: parent.width / 25
            columns: 1
            rows: 3
            opacity: 0.9
            spacing: height / 15

            Text {
                id: info_screen_anno_fileName
                font.pixelSize: parent.height / 5.5
                font.family: textFont.name
                width: parent.width
                color: "#888888"
                opacity: 1.0
                text: "Dateiname:"
            }

            Text {
                id: info_screen_anno_fileSize
                font.pixelSize: info_screen_anno_fileName.font.pixelSize
                font.family: textFont.name
                width: parent.width
                color: info_screen_anno_fileName.color
                opacity: 1.0
                text: "Bildgröße:"
            }

            Text {
                id: info_screen_anno_params
                font.pixelSize: info_screen_anno_fileName.font.pixelSize
                font.family: textFont.name
                width: parent.width
                color: info_screen_anno_fileName.color
                opacity: 1.0
                text: "Parameter:"
            }
        }

        Grid {
            id: info_screen_leftGrid
            height: parent.height - parent.height / 3
            anchors.verticalCenter: parent.verticalCenter
            anchors.verticalCenterOffset: info_screen_leftGrid_annotations.anchors.verticalCenterOffset
            anchors.left: info_screen_leftGrid_annotations.right
            anchors.right: info_screen_info_image.left
            anchors.rightMargin: parent.width / 25
            columns: 1
            rows: 3
            opacity: 0.9
            spacing: height / 15

            Text {
                id: info_screen_text_fileName
                font.pixelSize: parent.height / 5.5
                font.family: textFont.name
                width: parent.width
                color: "#FFFFFF"
                opacity: 1.0
                text: ""
                elide: Text.ElideRight
                maximumLineCount: 1
            }

            Text {
                id: info_screen_text_fileSize
                font.pixelSize: info_screen_text_fileName.font.pixelSize
                font.family: textFont.name
                width: parent.width
                color: "#FFFFFF"
                opacity: 1.0
                text: ""
                elide: Text.ElideRight
                maximumLineCount: 1
            }

            Text {
                id: info_screen_text_params
                font.pixelSize: info_screen_text_fileName.font.pixelSize
                font.family: textFont.name
                width: parent.width
                color: "#FFFFFF"
                opacity: 1.0
                text: ""
                elide: Text.ElideRight
                maximumLineCount: 1
            }
        }

        Grid {
            id: info_screen_rightGrid
            height: parent.height - parent.height / 3
            anchors.verticalCenter: parent.verticalCenter
            anchors.verticalCenterOffset: info_screen_leftGrid_annotations.anchors.verticalCenterOffset
            anchors.left: info_screen_info_image.right
            anchors.leftMargin: parent.width / 30
            anchors.right: parent.right
            anchors.rightMargin: parent.width / 30
            columns: 2
            rows: 2
            opacity: 0.9
            spacing: height / 15

            Text {
                id: info_screen_anno_description
                font.pixelSize: parent.height / 5.5
                font.family: textFont.name
                width: parent.width * 0.66
                color: "#888888"
                opacity: 1.0
                text: "Beschreibung:"
            }
            Text {
                id: info_screen_anno_date
                font.pixelSize: parent.height / 5.5
                font.family: textFont.name
                width: parent.width / 3
                color: "#888888"
                opacity: 1.0
                text: "Datum:"
            }

            Text {
                id: info_screen_text_description
                font.pixelSize: parent.height / 5
                font.family: textFont.name
                width: info_screen_anno_description.width
                color: "#FFFFFF"
                opacity: 1.0
                text: ""
                wrapMode: Text.Wrap
                elide: Text.ElideRight
                maximumLineCount: 2
            }

            Text {
                id: info_screen_text_date
                font.pixelSize: parent.height / 5
                font.family: textFont.name
                width: info_screen_anno_date.width
                color: "#FFFFFF"
                opacity: 1.0
                text: ""
                elide: Text.ElideRight
                maximumLineCount: 1
            }
        }
    }

    function show_hide_infobox() {
        fade_info_content.stop()

        if (info_screen_content.opacity === 0.0) {
            info_screen_content.opacity = 0.01

            update()
            reset_content_opacity()

            info_screen_info_image.opacity = 0.6
            info_screen_info_image.scale = 0.65

            fade_info.start()
        }
        else if (info_screen_content.opacity === 1.0) {
            fade_info_content.stop()
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

    function reset_content_opacity() {
        info_screen_imageNumber.opacity = 0.9
        info_screen_info_image.scale = 1.0
        info_screen_leftGrid.opacity = 0.9
        info_screen_rightGrid.opacity = 0.9
        info_screen_leftGrid_annotations = 0.9
    }

    function update_lang() {
        if (_settings_dialog.getLanguageForQml() === "en") {
            info_screen_anno_fileName.text = "Filename:"
            info_screen_anno_fileSize.text = "Size:"
            info_screen_anno_params.text = "Parameter:"
            info_screen_anno_description.text = "Description:"
            info_screen_anno_date.text = "Date:"
        }
        else {
            info_screen_anno_fileName.text = "Dateiname:"
            info_screen_anno_fileSize.text = "Bildgröße:"
            info_screen_anno_params.text = "Parameter:"
            info_screen_anno_description.text = "Beschreibung:"
            info_screen_anno_date.text = "Datum:"
        }

    }

    function update() {
        info_screen_imageNumber.text = _supervisor.getImageNumSlashTotalNumber()
        info_screen_text_fileName.text = _supervisor.getExifTagOfCurrent("fileName")
        info_screen_text_fileSize.text = _supervisor.getExifTagOfCurrent("resolutionAndSize")
        info_screen_text_params.text = _supervisor.getExifTagOfCurrent("acquisitionParameters")
        info_screen_text_description.text = _supervisor.getExifTagOfCurrent("imgDescription")
        info_screen_text_date.text = _supervisor.getExifTagOfCurrent("dateTimeOriginal")

        if (info_screen_text_params.text === "") info_screen_anno_params.visible = false;
        else info_screen_anno_params.visible = true;

        if (info_screen_text_description.text === "") info_screen_anno_description.visible = false;
        else info_screen_anno_description.visible = true;

        if (info_screen_text_date.text === "") info_screen_anno_date.visible = false;
        else info_screen_anno_date.visible = true;
    }

    function update_fade(duration) {
        transition_dummy.duration = duration
        fade_info_content.start()
    }

    NumberAnimation { id: transition_dummy; duration: 1000 }

    SequentialAnimation {
        id: fade_info_content
        running: false

        ParallelAnimation {
            NumberAnimation { target: info_screen_info_image; easing.type: Easing.InOutQuad; properties: "scale"; to: 0.9; duration: transition_dummy.duration / 2.5 }
            NumberAnimation { target: info_screen_imageNumber; properties: "opacity"; to: 0.0; duration: transition_dummy.duration / 2.5; easing.type: Easing.Linear }
            NumberAnimation { target: info_screen_leftGrid; properties: "opacity"; to: 0.0; duration: transition_dummy.duration / 2.5; easing.type: Easing.Linear }
            NumberAnimation { target: info_screen_rightGrid; properties: "opacity"; to: 0.0; duration: transition_dummy.duration / 2.5; easing.type: Easing.Linear }
            NumberAnimation { target: info_screen_leftGrid_annotations; properties: "opacity"; to: 0.0; duration: transition_dummy.duration / 2.5; easing.type: Easing.Linear }
        }

        ScriptAction { script: { update() } }

        ParallelAnimation {
            NumberAnimation { target: info_screen_imageNumber; properties: "opacity"; to: 0.9; duration: transition_dummy.duration / 2.5; easing.type: Easing.Linear }
            NumberAnimation { target: info_screen_info_image; easing.type: Easing.InOutQuad; properties: "scale"; to: 1.0; duration: transition_dummy.duration / 2.5 }
            NumberAnimation { target: info_screen_leftGrid; properties: "opacity"; to: 0.9; duration: transition_dummy.duration / 2.5; easing.type: Easing.Linear }
            NumberAnimation { target: info_screen_rightGrid; properties: "opacity"; to: 0.9; duration: transition_dummy.duration / 2.5; easing.type: Easing.Linear }
            NumberAnimation { target: info_screen_leftGrid_annotations; properties: "opacity"; to: 0.9; duration: transition_dummy.duration / 2.5; easing.type: Easing.Linear }
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
        NumberAnimation { target: info_screen_info_image; easing.overshoot: 5; easing.type: Easing.InOutBack; properties: "scale"; to: 1.0; duration: 500; }
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
        NumberAnimation { target: info_screen_info_image; easing.type: Easing.InQuad; properties: "scale"; to: 0.65; duration: 500; }

    }
}
