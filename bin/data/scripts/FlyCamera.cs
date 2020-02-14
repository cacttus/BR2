using System;
using BR2;

namespace BR2
{
  public class FlyingCameraController : CSharpScript
  {
    #region Private:Members
    private CameraNode _pCamera = null;
    //std::shared_ptr<RenderViewport> _pViewport = nullptr;
    //std::shared_ptr<CameraNode> _pCamera = nullptr;
    private vec2 _vMousePress = new vec3(0, 0, 0);
    private vec3 _vCamPos = new vec3(0, 0, 0);
    private vec3 _vMoveVel = new vec3(0, 0, 0);
    private float _fMaxMoveVel = 2.0f;
    private float _fMoveDamp = 9.0f;  //m/s

    private vec3 _vCamNormal = new vec3(0, 0, 0);
    private float _fPerUnitRotate = (float)M_PI / 2000.0f;
    private float _fRotationVelX = 0.0f; //current vel
    private float _fRotationVelY = 0.0f; //current vel
    private float _fRotationVelMin = -0.1f; //velocity min/max
    private float _fRotationVelMax = 0.1f;
    private float _fRotationEase = 15.50f; // % per second
    #endregion

    #region Public:Methods
    public void Start()
    {
      _pCamera = GetWorldObject();//Returns the WorldObject that this script is on.

      _pCamera = CameraNode::create(pv, pscene);
      _pCamera.getFrustum().setZFar(1000.0f); //We need a SUPER long zFar in order to zoom up to the tiles.  

      UpdateCameraPosition();

      _vMoveVel.Construct(0, 0, 0);
      _pCamera.SetPos(new vec3(30, 30, 30));
      _pCamera.Update(0.0f, std::map<Hash32, std::shared_ptr<Animator>>());//Make sure to create the frustum.
      _vCamNormal = _pCamera.getViewNormal();
      _vCamPos = _pCamera.getPos();


      _pCamera.setLookAt(new vec3(0, 0, 0));
      _pCamera.setPos(new vec3(0, 0, -10));
    }
    public void Update(float delta)
    {
      InputManager pInput = Gu.GetInputManager();

      //Capture & u8pdate input
      ButtonState::e eLmb = pInput.GetLmbState();
      vec2 vLast = pInput.GetLastMousePos();
      t_timeval us = Gu.GetMicroSeconds();

      UpdateRotate(pInput);

      //set cam pos
      //vec3 pos = _pCamera.getPos();
      _vCamPos += _vMoveVel;
      UpdateCameraPosition();

      //dampen the velocity
      vec3 v_n = new vec3(0,0,0);
      float v_len;
      _vMoveVel.len_and_norm(v_n, v_len);
      float v_new_len = v_len - v_len * _fMoveDamp * dt;
      v_new_len = MathUtils.Clamp(v_new_len, 0.0f, _fMaxMoveVel);
      _vMoveVel = v_n * v_new_len;

      //Finalluy update camera
      _pCamera.Update(dt, std::map<Hash32, std::shared_ptr<Animator>>());
    }
    public void SetActive()
    {
      Gu::setCamera(_pCamera);
    }
    public void SetPosAndLookAt(vec3 pos, vec3 lookat)
    {
      _vCamPos = pos;
      _vCamNormal = (lookat - pos).Normalize();

      UpdateCameraPosition();
    }
    public void UpdateTouch(InputManager pInput, float delta)
    {
      //Same as MoveCameraWSAD

      //Damp Slows us a bit when we zoom out.
      // float damp = fabsf(_fCamDist)*0.001f;
      float strafeAmt = 1.05; //fabsf(_fCamDist) / (CongaUtils::getNodeWidth() + damp) * factor;

      if (pInput.ShiftHeld())
      {
        strafeAmt = 2.1f;
      }

      strafeAmt *= delta;
      vec3 vel = new vec3(0, 0, 0);

      if (pInput.KeyPressOrDown(SDL_SCANCODE_W))
      {
        vel += _pCamera.GetViewNormal() * strafeAmt;
      }
      if (pInput.KeyPressOrDown(SDL_SCANCODE_S))
      {
        vel += _pCamera.GetViewNormal() * -strafeAmt;
      }
      if (pInput.KeyPressOrDown(SDL_SCANCODE_A))
      {
        vel += _pCamera.GetRightNormal() * -strafeAmt;
      }
      if (pInput.KeyPressOrDown(SDL_SCANCODE_D))
      {
        vel += _pCamera.GetRightNormal() * strafeAmt;
      }
      _vMoveVel += vel;

      UpdateCameraPosition();
    }
    #endregion

    #region Private: Methods
    private void UpdateCameraPosition()
    {
      if (_vCamNormal.squaredLength() == 0.0f)
      {
        //We have an error wit the camera normal because we're using the same normal from the
        //camera to do calculations.  This is a HACK:
        _vCamNormal.Construct(1.0f, 0, 0);
        Gu.DebugBreak();
      }
      vec3 vLookat = _vCamPos + _vCamNormal;
      _pCamera.SetPos(_vCamPos);
      _pCamera.SetLookAt(vLookat);
    }
    private void UpdateRotate(InputManager pInput)
    {
      vec2 vMouse = pInput.GetMousePos();
      ButtonState::e eRmb = pInput.GetRmbState();
      vec2 vDelta = new vec2(0, 0);
      if (eRmb == ButtonState::Press)
      {
        _vMousePress = vMouse;
      }
      if (eRmb == ButtonState::Press || eRmb == ButtonState::Down)
      {
        vDelta = vMouse - _vMousePress;
        pInput.WarpMouse((int)_vMousePress.x, (int)_vMousePress.y);
      }
      else if (eRmb == ButtonState::Release)
      {
      }

      if (vDelta.x != 0 || vDelta.y != 0)
      {
        RotateCameraNormal(_fPerUnitRotate * vDelta.x, _fPerUnitRotate * -vDelta.y);
      }
    }
    private void RotateCameraNormal(float rotX, float rotY)
    {
      vec3 camPos = GetScene().GetActiveCamera().GetPos();
      vec3 rt = _pCamera.GetRightNormal();
      vec3 up = _pCamera.GetUpNormal();

      vec3 vNewCamNormal = (_vCamNormal + rt * rotX + up * rotY).Normalize();

      float e = 0.0003;
      //#define N_INR(a, b) ((a >= (b-e)) && (a <= (b+e)))

      if (N_INR(vNewCamNormal.y, -1.0) || N_INR(vNewCamNormal.y, 1.0))
      {
        //Keep vCamNormal - don't let the user rotate to -1, or 1
      }
      else
      {
        _vCamNormal = vNewCamNormal;
      }

      //Additional checking to make sure we don't flip the UI
      if (_vCamNormal.y == 1.0f)
      {
        _vCamNormal.y = 0.98f;
        _vCamNormal.x = 0.01f;
        _vCamNormal.z = 0.01f;
        _vCamNormal.Normalize();
      }
      if (_vCamNormal.y == -1.0f)
      {
        _vCamNormal.y = -0.98f;
        _vCamNormal.x = -0.01f;
        _vCamNormal.z = -0.01f;
        _vCamNormal.Normalize();
      }

      //_vCamNormal = vec3(res.x, res.y, res.z);
      // vec3 lookAt = camPos + _vCamNormal;

      UpdateCameraPosition();
    }


    #endregion



  }
}