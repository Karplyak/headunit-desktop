import QtQuick 2.0
import Qt.labs.settings 1.0

Item {
    id: drawer
    FontLoader{id:ralewayRegular; source:"qrc:/qml/fonts/Raleway-Regular.ttf"}

    signal itemClicked(double frequency, var stationName)

    property var listTopMargin: 20
    property var listType: "favourite"
    property var activeStation: ""

    ListModel {
        id: stationModel
        ListElement {name:"Radio Pershe";favourite:true;frequency:88.2}
        ListElement {name:"Radio ROX";favourite:true;frequency:89.2}
        ListElement {name:"Galychyna";favourite:false;frequency:89.7}
        ListElement {name:"KISS FM";favourite:false;frequency:91.1}
        ListElement {name:"MELODIA FM";favourite:false;frequency:91.5}
        ListElement {name:"Radio Pyatnycya";favourite:false;frequency:92}
        ListElement {name:"Nostalgie";favourite:false;frequency:97}
        ListElement {name:"Lviv FM";favourite:false;frequency:100.8}
        ListElement {name:"Sokal FM";favourite:false;frequency:101.1}
        ListElement {name:"Craina";favourite:false;frequency:101.3}
        ListElement {name:"HIT FM";favourite:true;frequency:101.7}
        ListElement {name:"Maximum Fm";favourite:true;frequency:102.1}
        ListElement {name:"Radio Promin";favourite:false;frequency:102.5}

        ListElement {name:"Duzhe Radio";favourite:false;frequency:104.3}
        ListElement {name:"Radio JAZZ";favourite:false;frequency:105.4}
        ListElement {name:"Radio JAZZ";favourite:false;frequency:105.4}

        ListElement {name:"Lux FM Lviv";favourite:false;frequency:104.7}
        ListElement {name:"UKR 1 Radio";favourite:true;frequency:103.3}
    }

    Rectangle {
        color: "#212121"
        anchors.fill: parent
    }

    Item {
        id: item1
        anchors.fill: parent

        Item {
            id: buttons
            height: 40
            anchors.topMargin: listTopMargin
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.leftMargin: 0
            anchors.right: parent.right
            anchors.rightMargin: 0

            Rectangle {
                id: active_button_bg
                width: parent.width/2+spacer.width/2
                color: "#ffffff"
                anchors.top: parent.top
                anchors.topMargin: 0
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 0
                opacity: 0.5

                x:if(listType == "all"){
                    0
                } else if(listType == "favourite"){
                    buttons.width/2 - spacer.width/2
                }
                Behavior on x {

                    NumberAnimation {
                        duration: 600
                        easing.type: Easing.OutBounce
                    }
                }
            }

            Rectangle {
                id: all_button
                color: "#f44336"
                anchors.right: spacer.left
                anchors.rightMargin: 0
                anchors.top: parent.top
                anchors.topMargin: 6
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 6
                anchors.left: parent.left
                anchors.leftMargin: 6

                Text {
                    color: "#ffffff"
                    text: qsTr("All")
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    anchors.fill: parent
                    font.pixelSize: 14
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: listType="all"
                }
            }


            Item {
                id: spacer
                x: parent.width/2-width/2
                width: 6
                anchors.top: parent.top
                anchors.topMargin: 0
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 0
            }


            Rectangle {
                id: fav_button
                color: "#009688"
                anchors.left: spacer.right
                anchors.leftMargin: 0
                anchors.top: parent.top
                anchors.topMargin: 6
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 6
                anchors.right: parent.right
                anchors.rightMargin: 6

                Text {
                    color: "#ffffff"
                    text: qsTr("Favourites")
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    anchors.fill: parent
                    font.pixelSize: 14
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked:listType="favourite"
                }
            }


        }

        ListView {
            anchors.top: buttons.bottom
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.topMargin: 0
            clip: true
            model: stationModel
            delegate: RadioDrawerItem { }
        }

    }

    Component.onCompleted: {
        setPersonalInfoValue();
    }
}
