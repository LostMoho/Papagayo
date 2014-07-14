#include "mainwindow.h"
#include <QApplication>
#include <QFile>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	a.setApplicationName("Papagayo");
	a.setOrganizationName("Lost Marble");
	a.setOrganizationDomain("lostmarble.com");

	QCoreApplication::setAttribute(Qt::AA_DontShowIconsInMenus, true);

	MainWindow w;
	w.show();

	const QStringList arguments = a.arguments();
	if (arguments.size() > 1 && QFile::exists(arguments.last()))
	{
		w.OpenFile(arguments.last());
	}

	return a.exec();
}
