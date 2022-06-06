#ifndef ABOUT_H
#define ABOUT_H

class QString;

void displayDoc(const QString &url, const QString &title, bool runned_as_root = false);
void displayAboutMsgBox(const QString &title, const QString &message, const QString &licence_url,
                        const QString &license_title, bool runned_as_root = false);

#endif // ABOUT_H
