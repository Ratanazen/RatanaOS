import QtQuick 2.0;
import QtQuick.Controls 2.0;

Item {
    id: presentation
    anchors.fill: parent

    Timer {
        interval: 6000
        running: true
        repeat: true
        onTriggered: {
            swipeView.currentIndex = (swipeView.currentIndex + 1) % swipeView.count;
        }
    }

    SwipeView {
        id: swipeView
        anchors.fill: parent
        currentIndex: 0

        Item {
            id: slide1
            Column {
                anchors.centerIn: parent
                spacing: 16
                Image {
                    source: "squid.png"
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: 64
                    height: 64
                }
                Text {
                    text: "Welcome to RatanaOS 1.0 (macOS Sequoia Edition)"
                    color: "#FFFFFF"
                    font.pixelSize: 20
                    font.bold: true
                    anchors.horizontalCenter: parent.horizontalCenter
                }
                Text {
                    text: "An ultra-fast operating system with an elegant macOS desktop."
                    color: "#CCCCCC"
                    font.pixelSize: 13
                    anchors.horizontalCenter: parent.horizontalCenter
                }
            }
        }

        Item {
            id: slide2
            Column {
                anchors.centerIn: parent
                spacing: 16
                Text {
                    text: "RatanaOS Core & Package Ecosystem"
                    color: "#FFFFFF"
                    font.pixelSize: 20
                    font.bold: true
                    anchors.horizontalCenter: parent.horizontalCenter
                }
                Text {
                    text: "Access over 60,000 native packages and applications seamlessly."
                    color: "#CCCCCC"
                    font.pixelSize: 13
                    anchors.horizontalCenter: parent.horizontalCenter
                }
            }
        }

        Item {
            id: slide3
            Column {
                anchors.centerIn: parent
                spacing: 16
                Text {
                    text: "Modern Hardware Support"
                    color: "#FFFFFF"
                    font.pixelSize: 20
                    font.bold: true
                    anchors.horizontalCenter: parent.horizontalCenter
                }
                Text {
                    text: "Out-of-the-box drivers for Wi-Fi, GPUs, PipeWire audio, and NVMe storage."
                    color: "#CCCCCC"
                    font.pixelSize: 13
                    anchors.horizontalCenter: parent.horizontalCenter
                }
            }
        }
    }
}
