import QtQuick 2.9
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2

import HUDPlugins 1.0
import HUDTheme 1.0

ThemeRoot {
    id: __cclayout
    anchors.fill: parent
    property int left_seat_heat_rate: 0
    property int right_seat_heat_rate: 0
    property var activeStation: ""
    clip: true

    FontLoader{id:ralewayRegular; source:"qrc:/qml/fonts/Raleway-Regular.ttf"}

    function changeState(){
        if(__cclayout.state== ""){
            __cclayout.state="drawerOpen";
            radioDrawer.activeStation = activeStation;
            top_menu.menuButtonActive = true;
        } else{
            __cclayout.state="";
            top_menu.menuButtonActive = false;
        }
    }

    Item {
        id: main
        clip: true
        anchors.top: top_menu.bottom
        anchors.topMargin: 0
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.left: parent.left


        ColumnLayout {
            id: columnLayout1
            anchors.topMargin: 0
            anchors.bottomMargin: 0
            anchors.bottom: tuner.top
            anchors.right: parent.right
            anchors.left: parent.left
            anchors.top: parent.top
            spacing: 0


            Item {
                id: item1
                Layout.fillHeight: true
                Layout.fillWidth: true

                RowLayout {
                    id: signalStrength
                    anchors.top: parent.top
                    anchors.right: parent.right
                    anchors.topMargin: 15
                    anchors.rightMargin: 15
                    spacing: 2

                    Accessible.name: qsTr("Signal noise ratio: " + pluginContext.snr )

                    Rectangle{
                        height: 4
                        width: 4
                        color: (pluginContext.snr > 25) ? "green" : "dimgrey"
                        Accessible.ignored: true
                    }
                    Rectangle{
                        height: 8
                        width: 4
                        color: (pluginContext.snr > 45) ? "green" : "dimgrey"
                        Accessible.ignored: true
                    }
                    Rectangle{
                        height: 12
                        width: 4
                        color: (pluginContext.snr > 70) ? "green" : "dimgrey"
                        Accessible.ignored: true
                    }
                    Rectangle{
                        height: 16
                        width: 4
                        //color: (pluginContext.FMRadio.snr > 71) ? "green" : "dimgrey"
                        color: (pluginContext.snr > 95) ? "green" : "dimgrey"
                        Accessible.ignored: true
                    }

                    Rectangle{
                        height: 20
                        width: 4
                        color: (pluginContext.snr > 120) ? "green" : "dimgrey"
                        Accessible.ignored: true
                    }
                }

                ModeSwitch {
                    id: modeSwitch1
                    anchors.leftMargin: parent.height * 0.2
                    anchors.topMargin: parent.height * 0.2
                    anchors.bottomMargin: parent.height * 0.2
                    anchors.left: parent.left
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    width:height*2
                    rightText: "OFF"
                    leftText: "FM"
                    sliderColor: "#263238"
                }

                Text {
                    color: "#ffffff"
                    text: pluginContext.title.trim()
                    anchors.right: spacer1.left
                    anchors.rightMargin: 0
                    anchors.left: modeSwitch1.right
                    anchors.leftMargin: 0
                    anchors.top: parent.top
                    anchors.topMargin: 0
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: 0
                    verticalAlignment: Text.AlignBottom
                    horizontalAlignment: Text.AlignHCenter
                    font.pixelSize: 52
                    font.family: ralewayRegular.name
                }
                Item {
                    id: spacer1
                    width: 86.4
                    anchors.rightMargin: parent.height * 0.2
                    anchors.topMargin: parent.height * 0.2
                    anchors.bottomMargin: parent.height * 0.2
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    anchors.right: parent.right
                }



            }


            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true
                Text {
                    color: "#ffffff"
                    text: pluginContext.text.trim()
                    verticalAlignment: Text.AlignTop
                    font.family: ralewayRegular.name
                    horizontalAlignment: Text.AlignHCenter
                    anchors.fill: parent
                    font.pixelSize: 25
                }
            }


            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true

                RowLayout {
                    anchors.fill: parent
                    Item{
                        id: item5
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                        Item {
                            id: item2
                            width: parent.width/2
                            anchors.bottom: parent.bottom
                            anchors.bottomMargin: 0
                            anchors.top: parent.top
                            anchors.topMargin: 0
                            anchors.left: parent.left
                            anchors.leftMargin: 0

                            Image {
                                id: image1
                                fillMode: Image.PreserveAspectFit
                                anchors.fill: parent
                                source: "qrc:/qml/icons/svg/chevron-left.svg"
                                mipmap:true
                            }

                            MouseArea {
                                anchors.fill: parent
                                onClicked:tuner.moveFreqWith(-1)
                            }


                        }

                        Text {
                            id:frequencyText
                            color: "#ffffff"
                            text: Math.round(tuner.frequency * 10)/10
                            verticalAlignment: Text.AlignVCenter
                            font.family: ralewayRegular.name
                            horizontalAlignment: Text.AlignRight
                            anchors.fill: parent
                            font.pixelSize: height *0.8
                        }

                    }
                    Item{
                        id: item4
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        Text {
                            id: freqUnitText
                            color: "#ffffff"
                            text: " MHZ"
                            verticalAlignment: Text.AlignBottom
                            font.family: ralewayRegular.name
                            horizontalAlignment: Text.AlignLeft
                            anchors.fill: parent
                            font.pixelSize: height *0.6
                        }

                        Item {
                            id: item3
                            x: 0
                            width: parent.width/2
                            anchors.top: parent.top
                            anchors.topMargin: 0
                            anchors.bottom: parent.bottom
                            anchors.bottomMargin: 0
                            anchors.right: parent.right
                            anchors.rightMargin: 0
                            Image {
                                id: image2
                                anchors.fill: parent
                                source: "qrc:/qml/icons/svg/chevron-right.svg"
                                fillMode: Image.PreserveAspectFit
                                mipmap:true
                            }

                            MouseArea {
                                anchors.fill: parent

                                onClicked:{
                                    tuner.moveFreqWith(1)
                                    //pluginContext.openOverlay()
                                }
                            }
                        }
                    }

                }
            }



        }

        Tuner {
            id:tuner
            height: parent.height * 0.25
            anchors.left: parent.left
            anchors.leftMargin: 0
            anchors.right: parent.right
            anchors.rightMargin: 0
            anchors.bottom: parent.bottom
            anchors.bottomMargin: parent.height * 0.15
            onChanged:activeStation=""
        }
        Item {
            id: bottom1
            height: parent.height * 0.15
            anchors.left: parent.left
            anchors.leftMargin: 0
            anchors.right: parent.right
            anchors.rightMargin: 0
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 0

            Rectangle {
                color: "#212121"
                anchors.fill: parent
            }

            RowLayout {
                spacing: 0
                anchors.fill: parent
            }
        }

        Rectangle {
            id: overlay
            color: "#00000000"
            z: 10
            anchors.topMargin: 0
            visible: false
            anchors.top: parent.top
            anchors.right: parent.right
            anchors.rightMargin: 0
            anchors.left: parent.left
            anchors.leftMargin: 0
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 0
            opacity: 0

            MouseArea {
                anchors.fill: parent
                onClicked: changeState();
            }
        }

    }

    RadioDrawer{
        id: radioDrawer
        width: parent.width*0.3
        anchors.right: main.left
        anchors.rightMargin: 0
        anchors.top: parent.top
        anchors.topMargin: 0
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 0
        listTopMargin:top_menu.height
        onItemClicked: {
            tuner.moveToFreq(frequency);
            //pluginContext.moveToFreq(frequency*100);
            changeState();
            __cclayout.activeStation = stationName;
        }
    }


    states: [
        State {
            name: "drawerOpen"

            PropertyChanges {
                target: columnLayout1
                y: 100
                anchors.leftMargin: 0
            }

            PropertyChanges {
                target: main
                anchors.rightMargin: -parent.width*0.3
                anchors.leftMargin: parent.width*0.3
            }

            PropertyChanges {
                target: overlay
                color: "#000000"
                visible: true
                opacity: 0.5
            }

        }
    ]

    transitions: Transition {
        NumberAnimation { properties: "anchors.leftMargin,anchors.rightMargin,opacity,width"; duration: 250}
    }
    TopMenu{
        id: top_menu
        height: parent.height*0.10
        anchors.top: parent.top
        anchors.topMargin: 0
        anchors.left: parent.left
        anchors.leftMargin: 0
        anchors.right: parent.right
        anchors.rightMargin: 0
        onMenuClicked: changeState();
    }
    Connections{
        target: pluginContext
        Component.onCompleted: {
            //pluginContext.
            //tuner.moveToFreq(104.7); pluginContext.snr
            //frequency = pluginContext.lfrequency/100;
            tuner.moveToFreq(pluginContext.lfrequency/100);
                    //getFreq();
            //tuner.changed(frequency);
            //(frequency*100);
        }

        onShowErrorMessage:{
        }

        onShowInfoMessage:{
        }
    }
}
