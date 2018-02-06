/* ------------------------------------------------------------------
main.qml is part of picture-show2.
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

import QtQuick 2.0
import QtGraphicalEffects 1.0

Rectangle {
    id: root
    width: 1000
    height: 600
    color: "#222222"


// =================================================================================== //
// ----- DEFINITIONS ----- //
    FontLoader { id: titleFont; source: "qrc:///fonts/good_times.ttf" }
    FontLoader { id: textFont; source: "qrc:///fonts/pt_sans.ttf" }


// =================================================================================== //
// ----- FUNCTIONS ----- //
    function quit() {
        Qt.quit();
    }

    function refresh() {
        hp_screen.update()
        info_box.update_lang()
        message_box.update_preview_dimensions()
        setImageScale(image_slot1);
        setImageScale(image_slot2);
        setImageScale(image_slot3);

        if (_settings_dialog.getBackgroundColorQml() === 0) {
            root.color = "#000000"
            hp_screen.set_background_color("#000000")
            message_box.set_background_color("#000000")
            start_text.source = "qrc:///img/logoText.png"
        }
        else if (_settings_dialog.getBackgroundColorQml() === 1) {
            root.color = "#222222"
            hp_screen.set_background_color("#222222")
            message_box.set_background_color("#222222")
            start_text.source = "qrc:///img/logoText.png"
        }
        else if (_settings_dialog.getBackgroundColorQml() === 2) {
            root.color = "#555555"
            hp_screen.set_background_color("#555555")
            message_box.set_background_color("#555555")
            start_text.source = "qrc:///img/logoText.png"
        }
        else {
            root.color = "#FFFFFF"
            hp_screen.set_background_color("#FFFFFF")
            message_box.set_background_color("#FFFFFF")
            start_text.source = "qrc:///img/logoText_black.png"
        }
    }

    function animate_wait(anim) {
        if (anim)
            wait_animation.start()
        else
            wait_animation.stop()
    }

    function setImageScale(targetSlot)
    {
//        targetSlot.fillMode = Image.PreserveAspectCrop;
        if (_settings_dialog.getScaleTypeQml() === 1)
            targetSlot.fillMode = Image.PreserveAspectFit;
        else if (_settings_dialog.getScaleTypeQml() === 2)
            targetSlot.fillMode = Image.PreserveAspectCrop;

//        else if (targetSlot === "image_slot3")
//            targetSlot.fillMode = _settings_dialog.getScaleTypeQml();
    }

    function load_new_image(image, slot) {
        var patt = /^jumpto/;

        if (image === "current") {
            if (slot === 1) { image_slot1.source = ""; setImageScale(image_slot1); image_slot1.source = "image://pictures/current" }
            else if (slot === 2) { image_slot2.source = ""; setImageScale(image_slot2); image_slot2.source = "image://pictures/current" }
            else if (slot === 3) { image_slot3.source = ""; setImageScale(image_slot3); image_slot3.source = "image://pictures/current" }

        }
        else if (image === "next") {
            if (slot === 1) { image_slot1.source = ""; setImageScale(image_slot1); image_slot1.source = "image://pictures/next" }
            else if (slot === 2) { image_slot2.source = ""; setImageScale(image_slot2); image_slot2.source = "image://pictures/next" }
            else if (slot === 3) { image_slot3.source = ""; setImageScale(image_slot3); image_slot3.source = "image://pictures/next" }
        }
        else if (image === "prev") {
            if (slot === 1) { image_slot1.source = ""; setImageScale(image_slot1); image_slot1.source = "image://pictures/prev" }
            else if (slot === 2) { image_slot2.source = ""; setImageScale(image_slot2); image_slot2.source = "image://pictures/prev" }
            else if (slot === 3) { image_slot3.source = ""; setImageScale(image_slot3); image_slot3.source = "image://pictures/prev" }
        }
        else if (patt.test(image)) {
            message_box.load_jumpto_image(image)
        }
    }

    function waiting_to_one(slot_current) {
        if (slot_current === 1) { image_slot1.opacity = 0.01; start_to_first2_anim1.target = image_slot1; }
        if (slot_current === 2) { image_slot2.opacity = 0.01; start_to_first2_anim1.target = image_slot2; }
        if (slot_current === 3) { image_slot3.opacity = 0.01; start_to_first2_anim1.target = image_slot3; }
        start_to_first.start()
    }

    function show_to_waiting(slot_current) {
        if (image_slot1.opacity === 1.0) back_to_start_anim1.target = image_slot1;
        if (image_slot2.opacity === 1.0) back_to_start_anim1.target = image_slot2;
        if (image_slot3.opacity === 1.0) back_to_start_anim1.target = image_slot3;
        back_to_start.start()
    }

    function image_transition(slot_start, slot_end, forward) {
        transition_start.duration = _settings_dialog.getCurrentFadeLengthForQml();
        transition_stop.duration = _settings_dialog.getCurrentFadeLengthForQml();

        if (slot_start === 1) transition_start.target = image_slot1;
        if (slot_start === 2) transition_start.target = image_slot2;
        if (slot_start === 3) transition_start.target = image_slot3;
        if (slot_end === 1) { image_slot1.opacity = 0.01; transition_stop.target = image_slot1; }
        if (slot_end === 2) { image_slot2.opacity = 0.01; transition_stop.target = image_slot2; }
        if (slot_end === 3) { image_slot3.opacity = 0.01; transition_stop.target = image_slot3; }

        if (_settings_dialog.getBlendTypeForQml() === "FADE") {
            transition_anim_blend.start();
            info_box.update_fade(transition_start.duration)
        }
        else if (_settings_dialog.getBlendTypeForQml() === "FADE_BLACK") {
            transition_anim_blendOverBlack.start();
            info_box.update_fade(transition_start.duration)
        }
        else if (_settings_dialog.getBlendTypeForQml() === "SLIDE") {
            if (forward) transition_anim_slide_forward.start(); else transition_anim_slide_backward.start();
            info_box.update_fade(transition_start.duration * 2)
        }
        else if (_settings_dialog.getBlendTypeForQml() === "TWIST_FADE") {
            if (forward) transition_anim_twist_forward.start(); else transition_anim_twist_backward.start();
            info_box.update_fade(transition_start.duration)
        }
        else if (_settings_dialog.getBlendTypeForQml() === "ANDROID_STYLE") {
            if (forward) transition_anim_android_forward.start(); else transition_anim_android_backward.start();
            info_box.update_fade(transition_start.duration)
        }
        else {
            if (forward) transition_anim_hard.start(); else transition_anim_hard.start();
            info_box.update_fade(0)
        }

    }

    function show_help() {
        hp_screen.show_and_hide()
    }

    function start_panorama() {
        if (image_slot1.opacity === 1.0) transition_start.target = image_slot1;
        if (image_slot2.opacity === 1.0) transition_start.target = image_slot2;
        if (image_slot3.opacity === 1.0) transition_start.target = image_slot3;

        if (transition_start.target.fillMode === Image.PreserveAspectFit && transition_start.target.paintedHeight < root.height - 150) {
            panorama_animation_start.start()
        }
        else if (transition_start.target.fillMode === Image.PreserveAspectCrop && ((transition_start.target.sourceSize.width / transition_start.target.sourceSize.height) * root.height) > root.width + 150) {
            transition_start.target.fillMode = Image.PreserveAspectFit
            transition_start.target.scale = root.height / transition_start.target.paintedHeight
            panorama_animation_start.start()

//            if (_settings_dialog.getLanguageForQml() === "de")
//                _supervisor.infoMessage("Panorama-Modus", "Der Panorama-Modus kann nur bei Skalierung auf \"ganzes Bild\" verwendet werden!")
//            else
//                _supervisor.infoMessage("Panorama mode", "Panorama mode can only be used when image scaling is turned to \"whole image\"!")
        }
        else {
            if (_settings_dialog.getLanguageForQml() === "de")
                _supervisor.panoramaError("Panorama-Modus", "Das Bild hat kein Panoramaformat!")
            else
                _supervisor.panoramaError("Panorama mode", "The picture has no wide screen panorama format!")
        }
    }

    function stop_panorama() {
        if (image_slot1.opacity === 1.0) transition_start.target = image_slot1;
        if (image_slot2.opacity === 1.0) transition_start.target = image_slot2;
        if (image_slot3.opacity === 1.0) transition_start.target = image_slot3;

        if (transition_start.target.anchors.horizontalCenterOffset !== 0) {
            if (_settings_dialog.getScaleTypeQml() === 2)
                panorama_animation_stop_scale_anim.to = transition_start.target.scale;
            else
                panorama_animation_stop_scale_anim.to = 1.0;

            panorama_animation_stop.start();
        }
        else {
            _supervisor.overlayTransitionFinished()
        }
    }

    function show_hide_info() {
        info_box.show_hide_infobox()
    }

    function show_hide_bubble(text, timeout) {
        bubble_box.show_hide_bubble(text, timeout)
    }

    function show_jumpto_preview() {
        message_box.blend_jumpto();
    }

    function fade_preview_to_screen(current_slot) {
        if (image_slot1.opacity === 1.0) transition_start.target = image_slot1;
        if (image_slot2.opacity === 1.0) transition_start.target = image_slot2;
        if (image_slot3.opacity === 1.0) transition_start.target = image_slot3;

        if (current_slot === 1) transition_stop.target = image_slot1;
        if (current_slot === 2) transition_stop.target = image_slot2;
        if (current_slot === 3) transition_stop.target = image_slot3;

        fade_preview_to_big.start()
    }


// =================================================================================== //
// ----- ELEMENTS ----- //

    Item {
        id: jumpto_dummy
        visible: false
        height: root.height
        width: root.width
    }

    Image {
        id: start_logo
        width: 249
        height: 249
        source: "qrc:///img/start_logo.png"
        anchors.centerIn: parent;
        smooth: true;
        scale: 0.0

        NumberAnimation on scale { to: 1.0; duration: 1000; easing.type: Easing.OutCubic }
        NumberAnimation on rotation { to: 180.0; duration: 1000; easing.type: Easing.OutCubic }
    }

    Image {
        id: start_text
        width: root.height < 600 ? root.height-96 : 504
        height: root.height < 600 ? width*0.11 : 59
        source: "qrc:///img/logoText.png"
        anchors.centerIn: parent;
        anchors.verticalCenterOffset: 0
        opacity: 0.0
        smooth: true
        scale: 0.0

        NumberAnimation on opacity { to: 0.7; duration: 1000; easing.type: Easing.OutCubic }
        NumberAnimation on scale { to: 1.0; duration: 1000; easing.type: Easing.OutCubic }
        NumberAnimation on anchors.verticalCenterOffset { to: 200; duration: 1000; easing.type: Easing.OutCubic }
    }

    Image {
        id: image_slot1
        width: root.width
        height: root.height
        anchors.centerIn: parent
//        anchors.horizontalCenterOffset: (((sourceSize.width / sourceSize.height) * parent.height) - parent.width) / 2
        fillMode: Image.PreserveAspectCrop
        smooth: true
        cache: false
        visible: true
        opacity: 0.0
        scale: 1.0

        onStatusChanged: if (image_slot1.status === Image.Ready) _supervisor.imageLoadingFinished(source)

    }

    Image {
        id: image_slot2
        width: root.width
        height: root.height
        anchors.centerIn: parent
//        anchors.horizontalCenterOffset: (((sourceSize.width / sourceSize.height) * parent.height) - parent.width) / 2
        fillMode: Image.PreserveAspectFit
        smooth: true
        cache: false
        visible: true
        opacity: 0.0

        onStatusChanged: if (image_slot2.status === Image.Ready) _supervisor.imageLoadingFinished(source)

    }

    Image {
        id: image_slot3
        width: root.width
        height: root.height
        anchors.centerIn: parent
//        anchors.horizontalCenterOffset: (((sourceSize.width / sourceSize.height) * parent.height) - parent.width) / 2
        fillMode: Image.PreserveAspectFit
        smooth: true
        cache: false
        visible: true
        opacity: 0.0

        onStatusChanged: if (image_slot3.status === Image.Ready) _supervisor.imageLoadingFinished(source)

    }

    Infobox {
        id: info_box
        width: root.width;
        height: root.height
        z: 5
    }

    Helpscreen {
        id: hp_screen
        width: root.width
        height: root.height
        z: 5
    }

    Messagebox {
        id: message_box
        objectName: "messageBox"
        width: root.width
        height: root.height
        z: 5
    }

    Image {
        id: image_jumpto_big
        height: root.width * 0.65 > root.height ? (root.height / 6) * 1.35 : ((root.width * 0.65)  / 6) * 1.35;
        width: _settings_dialog.getScaleTypeQml() === 2 ? height * (root.width / root.height) : height * 1.7
//        width: height * 1.7
        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.horizontalCenterOffset: root.width * 0.3
        fillMode: _settings_dialog.getScaleTypeQml() === 2 ? Image.PreserveAspectCrop : Image.PreserveAspectFit
//        fillMode: Image.PreserveAspectFit
        smooth: true
        opacity: 0.0
        scale: 0.5
        visible: false
        z: 5
    }


    BubbleMessage {
        id: bubble_box
        width: root.width
        height: root.height
        z: 5
    }

// =================================================================================== //
// v----- ANIMATIONS -----v //

    NumberAnimation { id: transition_start; target: image_slot1; duration: 1000 }
    NumberAnimation { id: transition_stop; target: image_slot2; duration: 1000 }

    SequentialAnimation {
        id: wait_animation
        running: false
        alwaysRunToEnd: true
        loops: Animation.Infinite

        onStopped: {
             _supervisor.waitingFinished()
        }

        NumberAnimation { duration: 300 }
        NumberAnimation { target: start_logo; property: "scale"; duration: 500; to: 0.85; easing.type: Easing.OutQuad }
        NumberAnimation { target: start_logo; property: "scale"; easing.overshoot: 7; duration: 300; to: 1.0; easing.type: Easing.OutBack }
    }

    SequentialAnimation {
        id: start_to_first
        running: false

        onRunningChanged: {
            if (!start_to_first.running) {
                _supervisor.startShowFinished()
                _supervisor.blendingFinished()
            }
        }

        ParallelAnimation {
            NumberAnimation { target: start_text;  properties: "opacity"; to: 0.0; duration: 1000; easing.type: Easing.InCubic }
            NumberAnimation { target: start_logo;  properties: "opacity"; to: 0.0; duration: 1000; easing.type: Easing.InCubic }
            NumberAnimation { target: start_logo;  properties: "rotation"; to: 360.0; duration: 1000; easing.type: Easing.InCubic }

            ScriptAction { script: { start_to_first2_anim1.target.opacity = 0.01; start_to_first2_anim1.target.scale = 0.0; start_to_first2_anim1.target.rotation = 0} }

            NumberAnimation { id: start_to_first2_anim1; target: image_slot1;  properties: "opacity"; to: 1.0; duration: 1000; easing.type: Easing.InExpo }
            NumberAnimation { target: start_to_first2_anim1.target; easing.overshoot: 2.0;  properties: "scale"; to: 1.0; duration: 1500; easing.type: Easing.InOutBack }
        }
    }

    SequentialAnimation {
        id: back_to_start
        running: false

        onRunningChanged: {
            if (!back_to_start.running) {
                _supervisor.blendingFinished()
            }
        }

        ParallelAnimation {
            NumberAnimation { id: back_to_start_anim1; target: image_slot1;  properties: "opacity"; to: 0.0; duration: 1000; easing.type: Easing.InOutQuad }
            NumberAnimation { target: back_to_start_anim1.target;  properties: "scale"; to: 0.1; duration: 1000; easing.type: Easing.InQuart }
            NumberAnimation { target: start_text;  properties: "opacity"; to: 1.0; duration: 1000; easing.type: Easing.InCubic }
            NumberAnimation { target: start_logo;  properties: "opacity"; to: 1.0; duration: 1000; easing.type: Easing.InCubic }
            NumberAnimation { target: start_logo;  properties: "rotation"; to: 180.0; duration: 1000; easing.type: Easing.OutCubic }
        }

        ScriptAction { script: { back_to_start_anim1.target.scale = 1.0 } }
    }

    // v----- HARD TRANSITION -----v //

    ParallelAnimation {
        id: transition_anim_hard
        running: false

        onRunningChanged: {
            if (!transition_anim_hard.running) {
                _supervisor.blendingFinished()
            }
        }

        NumberAnimation { target: transition_start.target; property: "opacity"; to: 0.0; duration: 10; }
        NumberAnimation { target: transition_stop.target; property: "opacity"; to: 1.0; duration: 10; }
    }

    // ^----- HARD TRANSITION -----^ //

    // v----- BLEND TRANSITION -----v //

    ParallelAnimation {
        id: transition_anim_blend
        running: false

        onStopped: {
            _supervisor.blendingFinished()
        }

        NumberAnimation { target: transition_start.target;  properties: "opacity"; to: 0.0; duration: transition_start.duration; easing.type: Easing.InOutQuad }
        NumberAnimation { target: transition_stop.target;  properties: "opacity"; to: 1.0; duration: transition_stop.duration; easing.type: Easing.InOutQuad }
    }

    // ^----- BLEND TRANSITION -----^ //

    // v----- BLEND TRANSITION -----v //

    SequentialAnimation {
        id: transition_anim_blendOverBlack
        running: false

        onStopped: {
            _supervisor.blendingFinished()
        }

        NumberAnimation { target: transition_start.target;  properties: "opacity"; to: 0.0; duration: transition_start.duration / 2; easing.type: Easing.InOutQuad }
        NumberAnimation { target: transition_stop.target;  properties: "opacity"; to: 1.0; duration: transition_stop.duration / 2; easing.type: Easing.InOutQuad }
    }

    // ^----- BLEND TRANSITION -----^ //

    // v----- SLIDE TRANSITION -----v //

    SequentialAnimation {
        id: transition_anim_slide_forward
        running: false

        onRunningChanged: {
            if (!transition_anim_slide_forward.running) {
                _supervisor.blendingFinished()
            }
        }

        ScriptAction { script: { transition_stop.target.scale = 0.9; transition_stop.target.opacity = 0.7; transition_stop.target.anchors.horizontalCenterOffset = root.width} }

        ParallelAnimation {
            NumberAnimation { target: transition_start.target;  properties: "opacity"; to: 0.7; duration: transition_start.duration / 3; easing.type: Easing.InOutQuad }
            NumberAnimation { target: transition_start.target;  properties: "scale"; to: 0.9; duration: transition_start.duration / 3; easing.type: Easing.InOutQuad }
        }

        ParallelAnimation {
            NumberAnimation { target: transition_start.target;  properties: "anchors.horizontalCenterOffset"; to: -root.width; duration: transition_start.duration; easing.type: Easing.InOutQuart }
            NumberAnimation { target: transition_stop.target;  properties: "anchors.horizontalCenterOffset"; to: 0; duration: transition_stop.duration; easing.type: Easing.InOutQuart }
        }

        ParallelAnimation {
            NumberAnimation { target: transition_stop.target; properties: "opacity"; to: 1.0; duration: transition_stop.duration / 5; easing.type: Easing.InOutQuad }
            NumberAnimation { target: transition_stop.target; properties: "scale"; to: 1.0; duration: transition_stop.duration / 3; easing.type: Easing.InOutQuad }

        }

        ScriptAction { script: { transition_start.target.anchors.horizontalCenterOffset = 0; transition_start.target.scale = 1.0; transition_start.target.opacity = 0.0} }
    }

    SequentialAnimation {
        id: transition_anim_slide_backward
        running: false

        onRunningChanged: {
            if (!transition_anim_slide_backward.running) {
                _supervisor.blendingFinished()
            }
        }

        ScriptAction { script: { transition_stop.target.scale = 0.9; transition_stop.target.opacity = 0.7; transition_stop.target.anchors.horizontalCenterOffset = -root.width} }

        ParallelAnimation {
            NumberAnimation { target: transition_start.target;  properties: "opacity"; to: 0.7; duration: transition_start.duration / 3; easing.type: Easing.InOutQuad }
            NumberAnimation { target: transition_start.target;  properties: "scale"; to: 0.9; duration: transition_start.duration / 3; easing.type: Easing.InOutQuad }
        }

        ParallelAnimation {
            NumberAnimation { target: transition_start.target;  properties: "anchors.horizontalCenterOffset"; to: root.width; duration: transition_start.duration; easing.type: Easing.InOutQuart }
            NumberAnimation { target: transition_stop.target;  properties: "anchors.horizontalCenterOffset"; to: 0; duration: transition_stop.duration; easing.type: Easing.InOutQuart }
        }

        ParallelAnimation {
            NumberAnimation { target: transition_stop.target; properties: "opacity"; to: 1.0; duration: transition_stop.duration / 5; easing.type: Easing.InOutQuad }
            NumberAnimation { target: transition_stop.target; properties: "scale"; to: 1.0; duration: transition_stop.duration / 3; easing.type: Easing.InOutQuad }

        }

        ScriptAction { script: { transition_start.target.anchors.horizontalCenterOffset = 0; transition_start.target.scale = 1.0; transition_start.target.opacity = 0.0} }
}
    // ^----- SLIDE TRANSITION -----^ //

    // v----- SLIDE + FADE TRANSITION -----v //

    SequentialAnimation {
        id: transition_anim_slide2_forward
        running: false

        onRunningChanged: {
            if (!transition_anim_slide2_forward.running) {
                _supervisor.blendingFinished()
            }
        }

        ScriptAction { script: { transition_stop.target.scale = 0.8; transition_stop.target.opacity = 0.7; transition_start.target.z = 1; transition_stop.target.z = 0 } }

        ParallelAnimation {
            NumberAnimation { target: transition_start.target;  properties: "opacity"; to: 0.0; duration: transition_start.duration; easing.type: Easing.InQuad }
            NumberAnimation { target: transition_start.target;  properties: "anchors.horizontalCenterOffset"; to: -root.width / 10; duration: transition_start.duration; easing.type: Easing.InQuad }
            NumberAnimation { target: transition_stop.target; properties: "opacity"; to: 1.0; duration: transition_stop.duration; easing.type: Easing.InOutQuad }
            NumberAnimation { target: transition_stop.target; properties: "scale"; to: 1.0; duration: transition_stop.duration; easing.type: Easing.OutQuad }
        }

        ScriptAction { script: { transition_start.target.anchors.horizontalCenterOffset = 0; transition_start.target.scale = 1.0; transition_start.target.opacity = 0.0; transition_start.target.z = 0; transition_stop.target.z = 0 } }
    }

    SequentialAnimation {
        id: transition_anim_slide2_backward
        running: false

        onRunningChanged: {
            if (!transition_anim_slide2_backward.running) {
                _supervisor.blendingFinished()
            }
        }

        ScriptAction { script: { transition_stop.target.scale = 0.8; transition_stop.target.opacity = 0.7; transition_start.target.z = 1; transition_stop.target.z = 0  } }

        ParallelAnimation {
            NumberAnimation { target: transition_start.target;  properties: "opacity"; to: 0.0; duration: transition_start.duration; easing.type: Easing.InQuad }
            NumberAnimation { target: transition_start.target;  properties: "anchors.horizontalCenterOffset"; to: root.width / 10; duration: transition_start.duration; easing.type: Easing.InQuad }
            NumberAnimation { target: transition_stop.target; properties: "opacity"; to: 1.0; duration: transition_stop.duration; easing.type: Easing.InOutQuad }
            NumberAnimation { target: transition_stop.target; properties: "scale"; to: 1.0; duration: transition_stop.duration; easing.type: Easing.OutQuad }
        }

        ScriptAction { script: { transition_start.target.anchors.horizontalCenterOffset = 0; transition_start.target.scale = 1.0; transition_start.target.opacity = 0.0; ; transition_start.target.z = 0; transition_stop.target.z = 0 } }
}
    // ^----- SLIDE + FADE TRANSITION -----^ //

// DREHEN UND BLENDEN VOR
    SequentialAnimation {
        id: transition_anim_twist_forward
        running: false

        onRunningChanged: {
            if (!transition_anim_twist_forward.running) {
                _supervisor.blendingFinished()
            }
        }

        ScriptAction { script: { transition_stop.target.scale = 0.8; } }

        ParallelAnimation {
            NumberAnimation { target: transition_start.target;  properties: "opacity"; to: 0.0; duration: transition_start.duration; easing.type: Easing.InOutQuad }
            NumberAnimation { target: transition_stop.target; properties: "opacity"; to: 1.0; duration: transition_stop.duration; easing.type: Easing.InQuad }
            NumberAnimation { target: transition_start.target;  properties: "rotation"; to: 25; duration: transition_start.duration; easing.type: Easing.InQuad }
            NumberAnimation { target: transition_start.target;  properties: "scale"; to: 2.0; duration: transition_start.duration; easing.type: Easing.InQuad }
            NumberAnimation { target: transition_stop.target;  properties: "scale"; to: 1.0; duration: transition_stop.duration; easing.type: Easing.OutQuad }
        }

        ScriptAction { script: { transition_start.target.rotation = 0; transition_start.target.scale = 1.0;  } }
    }

// BLENDEN ZURÜCK
    SequentialAnimation {
        id: transition_anim_twist_backward
        running: false

        onRunningChanged: {
            if (!transition_anim_twist_backward.running) {
                _supervisor.blendingFinished()
            }
        }

        ScriptAction { script: { transition_stop.target.scale = 2.0; transition_stop.target.rotation = 25; } }

        ParallelAnimation {
            NumberAnimation { target: transition_start.target;  properties: "opacity"; to: 0.0; duration: transition_start.duration; easing.type: Easing.OutQuad }
            NumberAnimation { target: transition_stop.target; properties: "opacity"; to: 1.0; duration: transition_stop.duration; easing.type: Easing.InQuad }
            NumberAnimation { target: transition_stop.target;  properties: "rotation"; to: 0; duration: transition_start.duration; easing.type: Easing.OutQuad }
            NumberAnimation { target: transition_start.target;  properties: "scale"; to: 0.8; duration: transition_start.duration; easing.type: Easing.InQuad }
            NumberAnimation { target: transition_stop.target;  properties: "scale"; to: 1.0; duration: transition_stop.duration; easing.type: Easing.OutQuad }
        }

        ScriptAction { script: { transition_start.target.scale = 1.0; transition_start.target.rotation = 0; } }
    }

    // v----- ANDROID TRANSITION -----v //

    SequentialAnimation {
        id: transition_anim_android_forward
        running: false

        onRunningChanged: {
            if (!transition_anim_android_forward.running) {
                _supervisor.blendingFinished()
            }
        }

        ScriptAction {
            script: {
                transition_start.target.z = 1
                transition_stop.target.z = 0
                transition_stop.target.scale = 0.8;
                transition_stop.target.opacity = 0.0
            }
        }

        ParallelAnimation {
            NumberAnimation { target: transition_start.target;  properties: "anchors.horizontalCenterOffset"; to: -root.width; duration: transition_start.duration; easing.type: Easing.InQuad }
            NumberAnimation { target: transition_start.target;  properties: "opacity"; to: 0.6; duration: transition_start.duration; easing.type: Easing.InQuad }
            NumberAnimation { target: transition_stop.target;   properties: "scale"; to: 1.0; duration: transition_start.duration; easing.type: Easing.InOutQuad }
            NumberAnimation { target: transition_stop.target;   properties: "opacity"; to: 1.0; duration: transition_stop.duration; easing.type: Easing.InOutQuad }
        }

        ScriptAction {
            script: {
                transition_start.target.z = 0
                transition_stop.target.z = 0
                transition_start.target.anchors.horizontalCenterOffset = 0;
                transition_start.target.scale = 1.0;
                transition_start.target.opacity = 0.0
            }
        }
    }

    SequentialAnimation {
        id: transition_anim_android_backward
        running: false

        onRunningChanged: {
            if (!transition_anim_android_backward.running) {
                _supervisor.blendingFinished()
            }
        }

        ScriptAction {
            script: {
                transition_start.target.z = 0
                transition_stop.target.z = 1
                transition_stop.target.opacity = 0.7;
                transition_stop.target.anchors.horizontalCenterOffset = -root.width
            }
        }

        ParallelAnimation {
            NumberAnimation { target: transition_start.target;  properties: "scale"; to: 0.9; duration: transition_start.duration; easing.type: Easing.InQuad }
            NumberAnimation { target: transition_start.target;  properties: "opacity"; to: 0.0; duration: transition_start.duration; easing.type: Easing.InQuad }
            NumberAnimation { target: transition_stop.target;   properties: "anchors.horizontalCenterOffset"; to: 0; duration: transition_start.duration; easing.type: Easing.OutQuad }
            NumberAnimation { target: transition_stop.target;   properties: "opacity"; to: 1.0; duration: transition_stop.duration; easing.type: Easing.InOutQuad }
        }

        ScriptAction {
            script: {
                transition_start.target.z = 0
                transition_stop.target.z = 0
                transition_start.target.scale = 1.0;
                transition_start.target.opacity = 0.0
            }
        }
}
    // ^----- ANDROID TRANSITION -----^ //


// PANORAMA

    SequentialAnimation {
        id: panorama_animation_start
        running: false

        ParallelAnimation {
            NumberAnimation {   target: transition_start.target;
                                property: "anchors.horizontalCenterOffset";
                                easing.type: Easing.InOutQuad
                                to: (((transition_start.target.sourceSize.width / transition_start.target.sourceSize.height) * root.height) - root.width) / 2;
                                duration: 1000 }
            NumberAnimation {   target: transition_start.target;
                                property: "scale";
                                easing.type: Easing.InOutQuad
                                to: root.height / transition_start.target.paintedHeight
                                duration: 1000 }
        }

        ScriptAction { script: { panorama_animation1.start() } }

    }

    ParallelAnimation {
        id: panorama_animation1
        running: false
        loops: Animation.Infinite

        SequentialAnimation {
            NumberAnimation {   target: transition_start.target;
                                property: "anchors.horizontalCenterOffset";
                                easing.type: Easing.InOutSine
                                to: -(((transition_start.target.sourceSize.width / transition_start.target.sourceSize.height) * root.height) - root.width) / 2;
                                duration: /*(transition_start.target.sourceSize.width / transition_start.target.sourceSize.height) **/ 4000
                            }
            NumberAnimation {   target: transition_start.target;
                                property: "anchors.horizontalCenterOffset";
                                easing.type: Easing.InOutSine
                                to: (((transition_start.target.sourceSize.width / transition_start.target.sourceSize.height) * root.height) - root.width) / 2;
                                duration: /*(transition_start.target.sourceSize.width / transition_start.target.sourceSize.height) **/ 4000
            }
        }
    }

    SequentialAnimation {
        id: panorama_animation_stop
        running: false

        onRunningChanged: {
            if (!panorama_animation_stop.running) {
                _supervisor.overlayTransitionFinished()
            }
        }


        ScriptAction {
            script: {
                panorama_animation_start.stop();
                panorama_animation1.stop();
            }
        }

        ParallelAnimation {
            NumberAnimation {   target: transition_start.target;
                                property: "anchors.horizontalCenterOffset";
                                easing.type: Easing.InOutQuad
                                to: 0;
                                duration: 1000 }
            NumberAnimation {   id: panorama_animation_stop_scale_anim
                                target: transition_start.target;
                                property: "scale";
                                easing.type: Easing.InOutQuad
                                to: 1.0
                                duration: 1000 }
        }

        ScriptAction {
            script: {
                if (_settings_dialog.getScaleTypeQml() === 2) {
                   transition_start.target.fillMode = Image.PreserveAspectCrop
                   transition_start.target.scale = 1.0
                }
             }
        }
    }

    SequentialAnimation {
        id: fade_preview_to_big
        running: false

        onRunningChanged: {
            if (!fade_preview_to_big.running) {
                _supervisor.blendingFinished()
            }
        }

        ScriptAction {
            script: {
                image_jumpto_big.source = message_box.get_jumpto_source()
                image_jumpto_big.height = root.width * 0.65 > root.height ? (root.height / 6) * 1.35 : ((root.width * 0.65)  / 6) * 1.35;
                image_jumpto_big.width = _settings_dialog.getScaleTypeQml() === 2 ? image_jumpto_big.height * (root.width / root.height) : image_jumpto_big.height * 1.7
//                image_jumpto_big.width = image_jumpto_big.height * 1.7
                image_jumpto_big.anchors.horizontalCenterOffset = root.width * 0.3
                image_jumpto_big.fillMode = _settings_dialog.getScaleTypeQml() === 2 ? Image.PreserveAspectCrop : Image.PreserveAspectFit
//                image_jumpto_big.fillMode = Image.PreserveAspectFit
                image_jumpto_big.scale = 1.0
                image_jumpto_big.visible = true
                image_jumpto_big.opacity = 1.0

                if (info_box.isVisible())
                    info_box.show_hide_infobox()

                if (_settings_dialog.getScaleTypeQml() === 2) {
                    if ((image_jumpto_big.sourceSize.width / image_jumpto_big.sourceSize.height) > (root.width / root.height)) {
                        jumpto_dummy.height = root.height
                        jumpto_dummy.width = root.height * (image_jumpto_big.sourceSize.width / image_jumpto_big.sourceSize.height)
                    }
                    else {
                        jumpto_dummy.width = root.width
                        jumpto_dummy.height = root.width * (image_jumpto_big.sourceSize.height / image_jumpto_big.sourceSize.width)
                    }
                }
                else {
                    jumpto_dummy.height = root.height
                    jumpto_dummy.width = root.width
                }
            }
        }

        ParallelAnimation {
            NumberAnimation { target: image_jumpto_big; easing.type: Easing.InOutQuart; properties: "height"; to: root.height; duration: 800 }
            NumberAnimation { target: image_jumpto_big; easing.type: Easing.InOutQuart; properties: "width"; to: root.width; duration: 800 }
            NumberAnimation { target: image_jumpto_big; easing.type: Easing.InOutQuart; properties: "scale"; to: 1.0; duration: 800 }
            NumberAnimation { target: image_jumpto_big; easing.type: Easing.InOutQuart; properties: "anchors.horizontalCenterOffset"; to: 0; duration: 800 }
            NumberAnimation { target: transition_start.target;  properties: "opacity"; to: 0.0; duration: 800; easing.type: Easing.InOutQuad }
        }

        ScriptAction {
            script: {
//                message_box.show_hide_message("", "", "", false)
                transition_stop.target.opacity = 1.0
                image_jumpto_big.visible = false
                if (_supervisor.isInfoActive())
                    info_box.show_hide_infobox()
            }
        }
    }




}
