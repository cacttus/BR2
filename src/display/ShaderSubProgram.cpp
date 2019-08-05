#include "../base/BaseAll.h"
#include "../display/ShaderSubProgram.h"


namespace Game {
ShaderSubProgram::ShaderSubProgram() {
}
ShaderSubProgram::~ShaderSubProgram() {
}
void ShaderSubProgram::init(std::shared_ptr<GLContext> ctx, t_string loc, ShaderType::e type) {
    ctx->chkErrRt();
    GLenum eShaderType;

    //Parse shader by file ext
    if (type == ShaderType::e::st_use_extension) {
        _eType = getShaderTypeByFileLocation(loc);
    }
    else {
        _eType = type;
    }
    eShaderType = getGLShaderEnum(_eType);

    if (_compileErrors.size() > 0) {
        return;
    }

    //Create shader.
    _glId = ctx->glCreateShader(eShaderType);
    GLenum er = glGetError();
    if (er != GL_NO_ERROR) {
        setStatus(ShaderStatus::e::CreateFailed);
        _generalErrors.push_back( Stz "[error] Failed to create shader of type "+ 
            StringUtil::toHex((int)eShaderType, true)+ ". GL error: "+ StringUtil::toHex(er, true));
    }
    else {
        setSourceLocation(loc);
        setStatus(ShaderStatus::e::CreateComplete);
    }
}
t_string ShaderSubProgram::getHumanReadableErrorString() const
{
    t_string str;

    str += "Errors loading ";
    str += _sourceLocation;
    str += "\n";

    for (size_t i = 0; i<_compileErrors.size(); ++i)
    {
        str += "  ";
        str += _compileErrors[i];
        str += "\n";
    }
    return str;
}
void ShaderSubProgram::debugPrintShaderSource() const
{
    t_string str = "\n";
    str += Stz "Source For: " + _sourceLocation;
    str += "\n";
    for (size_t iLine = 0; iLine<_sourceLines.size(); ++iLine)
    {
        str += t_string(StringUtil::getZeroPaddedNumber((int32_t)iLine + 0, 4));
        str += " ";
        t_string nl = StringUtil::removeNewline(_sourceLines[iLine]);//.substr(0, _sourceLines[i].length() - 2);
        if(nl.length() != 0) {
            //Zero length strings will add \0 which will terminate.  
            str += nl;
        }
        str += t_string("\n");
    }
    BroLogDebug(str);
}
ShaderType::e ShaderSubProgram::getShaderTypeByFileLocation(DiskLoc& loc)
{
    ShaderType::e type = ShaderType::e::st_vertex;

    // - Get type by filename

    t_string other;
    size_t off = loc.rfind('.');

    AssertOrThrow2(off != t_string::npos);

    for (size_t x = off + 1; x<loc.size(); x++)
        other += loc.at(x);

    if (!(other.compare(t_string("vs"))))
        type = ShaderType::e::st_vertex;
    else if (!(other.compare(t_string("ps"))))
        type = ShaderType::e::st_fragment;
    else if (!(other.compare(t_string("fs"))))
        type = ShaderType::e::st_fragment;
    else if (!(other.compare(t_string("gs"))))
        type = ShaderType::e::st_geometry;
    else if (!(other.compare(t_string("cs"))))
        type = ShaderType::e::st_compute;
    else
        _generalErrors.push_back(Stz "[error] Shader file type '"+ other+ "' not recognized.");

    return type;
}
GLenum ShaderSubProgram::getGLShaderEnum(ShaderType::e type) {
    if (type == ShaderType::e::st_fragment)
        return GL_FRAGMENT_SHADER;
    else if (type == ShaderType::e::st_compute)
        return GL_COMPUTE_SHADER;
    else if (type == ShaderType::e::st_vertex)
        return GL_VERTEX_SHADER;
    else if (type == ShaderType::e::st_geometry)
        return GL_GEOMETRY_SHADER;
    else
        _generalErrors.push_back(Stz "[error] Opengl Shader type not supported for ShaderType::e = "+ (int)type);
    return GL_VERTEX_SHADER;
}




}//ns Game
