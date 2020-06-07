#pragma once

#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMouseEvent>

#include "preferences/usersettings.h"
#include "skin/skincontext.h"
#include "track/track.h"
#include "util/parented_ptr.h"
#include "widget/trackdroptarget.h"
#include "widget/wlabel.h"

class TrackCollectionManager;
class WTrackMenu;

class WTrackProperty : public WLabel, public TrackDropTarget {
    Q_OBJECT
  public:
    WTrackProperty(
            QWidget* pParent,
            UserSettingsPointer pConfig,
            TrackCollectionManager* pTrackCollectionManager,
            const char* group);
    ~WTrackProperty() override;

    void setup(const QDomNode& node, const SkinContext& context) override;

signals:
    void trackDropped(QString filename, QString group) override;
    void cloneDeck(QString source_group, QString target_group) override;

  public slots:
    void slotTrackLoaded(TrackPointer track);
    void slotLoadingTrack(TrackPointer pNewTrack, TrackPointer pOldTrack);

  protected:
    void contextMenuEvent(QContextMenuEvent* event) override;

  private slots:
    void slotTrackChanged(TrackId);

  private:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

    void updateLabel();

    const char* m_pGroup;
    const UserSettingsPointer m_pConfig;
    TrackPointer m_pCurrentTrack;
    QString m_property;

    const parented_ptr<WTrackMenu> m_pTrackMenu;
};
