#pragma once

#include <memory>

#include <QOpenGLDebugLogger>
#include <QMouseEvent>

#include <DirectionInputController.hpp>
#include <MotionInputController.hpp>

#include "Base/GLWindow.hpp"
#include "FirstSceneObject.hpp"
#include "FirstShader.hpp"

namespace Kononov {

class MainWindow final : public fgl::GLWindow {
  Q_OBJECT
public:
  void init() override;

  void render() override;

protected:
  void mousePressEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
  void keyPressEvent(QKeyEvent *event) override;
  void keyReleaseEvent(QKeyEvent *event) override;

public slots:
  static void onMessageLogged(const QOpenGLDebugMessage &message);

private:
  std::shared_ptr<DirectionInputController> m_look_dir;
  std::shared_ptr<MotionInputController> m_motion;

  std::unique_ptr<FirstSceneObject> m_skull;

  std::unique_ptr<QOpenGLDebugLogger> m_logger;

  // Frame counter for animation.
  int m_frame = 0;
};

} // namespace Kononov