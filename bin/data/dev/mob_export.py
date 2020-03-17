#
#    The BR Model Exporter
#    20161221 
#    20161221 
#    2017 12 04  ----  This is version 2 of FK export using actions instead of the 'defined' in the editor.
#    2017 12 06 - combined all stuff into one file - .mob
#    2017 12 15 Instead of exporting curves we're going to export keyframe granluarity  
#           basically you can add tons of modifiers to keyframes in blender
#           we can capture that by just exporting total matrices at a fine granularity.
#    2017 12 27 Fixed issues where we were exporting unparented, or unboned armatures.
#    2017 12 28 Added mesh triangulation (because, ugh..)
#    2017 12 29 Removed the GUI, Removed settings
#    2018 01 11 Added physics_body for physics, removed Special Meshes
#    2018 01 31 - fixed running this from the command line (now outpath defaults to ./)
#               - added environment map + mirror reflection
#    2018 02 23 - added transparency + glass With the following limits
#                       Material -> Transparency
#                           Alpha - used, IOR - used, Filter - used (for glass color)
#                           the followign must be set to equal blender
#                           Fresnel - 0.0, Falloff - 1.0, Limit - 0.0, Depth - 1, Amount - 1.0
#                       Textures
#                           For textures enable "Alpha" on the texture under 'influence' and move the sldier from [0,1] *this only works if you have *Transparency* enabled in the material.
#   BONE PARENTING
#    To use Bone Parenting, you must first select all the Child Objects you wish to parent to a specific Armature Bone, then Shift-RMB select the Armature
#    Object and switch it into Pose Mode and then select the specific bone you 
#    wish to be the Parent Bone by RMB selecting it. Once done press Ctrl-P and 
#    select Bone from the Set Parent To pop-up menu.
#
#   How To Use:
#       Note:  coordinates export LHS..
#       1 Select the ARMATURE (bones) before you export
#       2 SELECT ALL OBJECT(s)

#       3 Open BPY console
#       4 execute this with path pointed to this file
#       5 exec(compile(open("C:/git/dev/cw/bin/data/dev/mob_export.py").read(), "C:/git/dev/cw/bin/data/dev/mob_export.py", 'exec'))
#        CMD "C:\Users\squid\Desktop\APP\blender-2.78a-windows64\blender.exe" ".\dev\hand_curs_01.blend" --background --python ".\dev\mob_export.py" -- -outpath "./mob"
#       NOTE: Make sure that your actions are in the NLA editor, if they're just edited they won't export
#
#       
#
#
#   TO EXPORT NORMAL TEXTURES
#       Use the influence dropdown under texture - check "geometry -> normal"
#       Use the influence dropdown under texture - check "geometry -> normal"
# Blender Notes For Making Bones
#       create tab -> bones.
#       e = extrude (make) bone.
#       object mode
#       right click model (not bones)
#       modifiers tab -> add modifier -> armature
#       THEN hold shift - click on bones
#       Ctrl + P -> armature default automatic weights
#       Open Dope Sheet
#       go to pose mode -> select all  -> hit I -> LocRotScale
#       On Dope Sheet click "Action Editor"
#       In action Editor -> + sign - add new action - name it.
#       These are what get exported.

#        Before exporting
#            Bake transform
#                 Go to object mode
#                    Ctrl+A
#                        Bake Pos + Rot.
#                            Also bake location
#                                *Enter
#                                **Done
#
# Blender Notes For Tree Foliage
#        **Make sure to first duplicate the Plane you're using for foliage and flip normals (see below)
#           Deselect all - A
#            B - select a region of items
#            
#            CTRL+J - join objects into one mesh
#                Join tree foliage planes into one mesh
#                  Must be in object mode
#
#            Disable 2 sided materials
#                Main button bar
#                    Data
#                        Double Sided - uncheck
#
#               Flip Normals
#                  Go to Edit Mode
#                    Make sure all appropriate polygons are selected
#                        Left tab bar
#                            Shading / UVs Tab
#                                Normals Dropdown
#                                    Flip Direction                        
#
#

import bpy
import os
import math
import shutil
import time
import pprint
import bpy_extras.io_utils
from mathutils import Vector, Matrix
from bpy.props import IntProperty, EnumProperty, CollectionProperty, PointerProperty
from bpy.types import PropertyGroup, UIList, Panel, Operator
from bpy import context
import bpy_extras 
import bmesh
import sys, traceback
import argparse

#See 
#https://www.blender.org/api/blender_python_api_2_70_5/bpy.types.Operator.html
class MobExport:
    #strDefaultBasePath = "C:\\git\\dev\\cw\\bin\\data-dc\\mob"
    #strDefaultModelName =  #Gets filename from path.

    fVersion = 0.02
    
    blnMakeBackup = True
    
    strPathName = ""#strDefaultBasePath
    strModelName = ""#strDefaultModelName
    
    
    #don't enable calc normasl, because of flat faced.
    blnCalcNormals = False #Enabling this all the time because for test.
    blnFlipTris = False
    blnFlipUVs = False
    blnSaveTextures = True
    blnPRSKeyframes = True
    intGranularity = 1 # Multiply each keyframe by 3
    
    _iWarningCount = 0
    _iErrorCount = 0
    
    def __init__(self, strOutputPath):
        self.strPathName = strOutputPath
        self.strModelName = os.path.splitext(os.path.basename(bpy.data.filepath))[0]
        
    def export(self):
        #Select NOTHING
        bpy.context.scene.objects.active = None
        
        t_beg = self.millis()
        #Do not remove the $$ - we use that in the MobMaker to get the file name.
        print("exporting $1\"" + self.getMobFullPathWithFilename() + "\"$1..")

        #Create mob dir
        self.makePath(self.getMobFullPathWithFilename())

        #backup
        if self.blnMakeBackup == True:
            self.tryBackupFile()

        #export
        self.exportAllAsMob(self.getMobFullPathWithFilename())

        print("..done " + str(self.millis() - t_beg) + "ms");
        return
    #def getSettingsPath(self):
    #    return self.strDefaultBasePath + "BpyLastExportSettings.config"
    #def getModelName(self):
    #    return self.strModelName
    def getMobFullPathWithoutFilename(self):
        if self.strPathName[-1:] != "\\" and self.strPathName[-1:] != "/":
            self.strPathName += "/"
            
        basePath = self.strPathName
        
        if not os.path.exists(basePath):
            os.mkdir(basePath)
        #don't put .mob
        mobFolder = self.strModelName #+ ".mob"
        mobFullPath = os.path.join(basePath, mobFolder)
        
        return mobFullPath
    def getMobFullPathWithFilename(self):
        mobFileName = self.strModelName + ".mob"
        mobFullPath = self.getMobFullPathWithoutFilename()
        mobFullPathWithFilename = os.path.join(mobFullPath, mobFileName)

        return mobFullPathWithFilename        
    #Export all model data.
    def millis(self):
        return int(round(time.time() * 1000))

    # Okay, for now we're going to set the bind pose to be Keyframe 1
    def getVertexWeightsForMesh(self, ob_mesh):
        strFile = ""
        #https://docs.blender.org/api/blender_python_api_2_77_0/bpy.types.Modifier.html

        strFile += self.getVertexWeightsForMeshArm(ob_mesh)
                    
        return strFile
    def exportAllActionNames(self):
        strActions = "\n"
        strActions += "#All Actions, tells us what actions are in this file without ctrl+f\n"
        for ob in bpy.data.objects:
            #select the object
            bpy.context.scene.objects.active = ob
            bpy.data.objects[ob.name].select = True   
             
            if ob.type == 'ARMATURE' or ob.type == 'MESH':
                if ob.animation_data != None:
                    for nla in ob.animation_data.nla_tracks:
                        nla.select = True
                        for strip in nla.strips:
                            iMinKeyf = self.getMinKeyframeForAction(strip.action)
                            iMaxKeyf = self.getMaxKeyframeForAction(strip.action)
                            strActions += "action "+str(ob.type)+" \"" + self.getObjectName(strip.action, None) + "\" " +str(iMinKeyf)+ " " +str(iMaxKeyf)+ "\n"
                        
        return strActions        
    def exportAllObjectNames(self):
        strObjects = "\n"
        strObjects += "#All Objects\n"
        for ob in bpy.data.objects:
            #select the object
            bpy.context.scene.objects.active = ob
            bpy.data.objects[ob.name].select = True   
             
            if ob.type == 'ARMATURE' or ob.type == 'MESH':
                strObjects += "object " + str(ob.type) + " \"" + self.getObjectName(ob, None) + "\" " + str(ob.is_visible(bpy.context.scene)) + "\n"
                        
        return strObjects
    def getArmatureId(self, ob_arm):
        id = 0
        for ob in bpy.data.objects:
            if ob.type == 'ARMATURE':
                if ob == ob_arm:
                    return id;
                id += 1
        return id
    def getParentInverse(self, ob):
        #the reason for this is that blener has inverse parent mat data,e ven when/
        #ob has no parent - thus the data is invalid.  So return the identity matrix
        parentMatrix = Matrix.Identity(4)
        if ob != None:
            if ob.parent != None:
                parentMatrix = ob.matrix_parent_inverse
        return parentMatrix
    def exportAllArmatures(self):
        strFile = ""
        for ob in bpy.data.objects:
            #select the object
            bpy.context.scene.objects.active = ob
            bpy.data.objects[ob.name].select = True   
            
            if not ob:
                continue
            elif ob.type == 'ARMATURE':
                #Export the armature bone relationships.
                #bone "name" index "parent" vbeg vend bpoi bpjr
                # Bone Hierarchy
                if self.isValidArmature(ob):
                
                    strParent = self.getParentString(ob)

                    strFile += "\n"
                    strFile += "arm_beg "+"\"" + self.getObjectName(ob, None) + "\" " + strParent +" " + str(self.getArmatureId(ob)) + "\n"
                    bpy.ops.object.mode_set(mode='OBJECT')
                    strFile += "arm_world " + self.matToString(self.glMat4(ob.matrix_world)) + "\n"
                    strFile += "arm_parent_inverse " + self.matToString(self.glMat4(self.getParentInverse(ob))) + "\n"
                    
                    #Keyframe must be bind pose, or else head/tail will be wrong  For this we set the first key frame to a very far away
                    #https://blender.stackexchange.com/questions/15170/blender-python-exporting-bone-matrices-for-animation-relative-to-parent
                    bpy.ops.object.mode_set(mode='EDIT') #Must be in edit mode for bind pose
                    bpy.data.objects[ob.name].select = True 
                    strFile += self.getAllBones(ob, ob.pose.bones) #"hc " + self.getBoneHierarchyString(ob_arm.pose.bones) + "\n"
                    #From gran 6
                    #strFile += self.getAllBoneBind(ob, ob.data.bones) #"hc " + self.getBoneHierarchyString(ob_arm.pose.bones) + "\n"

                    strFile += "arm_end \"" + self.getObjectName(ob, None) + "\"\n\n"
            
        return strFile
    def isValidArmature(self, ob_arm):
        #Arm is bound to meshes
        ch = [child for child in ob_arm.children if child.type == 'MESH' and child.find_armature()]
        if len(ch) == 0:
            self.emitError("Armature " + ob_arm.name + " has no bound meshes ");
            return False
        #Arm has more than 1 bone
        if len(ob_arm.data.bones) == 0:
            self.emitError("Armature " + ob_arm.name + " has no bones ");
            return False
        
        return True
    def exportAllKeyframes(self):
        strFile = ""
        for ob in bpy.data.objects:
            if ob.hide == False:
                #select the object
                bpy.context.scene.objects.active = ob
                bpy.data.objects[ob.name].select = True   

                #Export everything for the object.
                #Avoid trying to export random ass animations.
                #strFile += "#KF [index] M44 [16 values] Curve xy, Curve mode, easing, left bezier handle xy, type, right bezier handle xy, type\n"
                if ob.type == 'ARMATURE' or ob.type == 'MESH':
                    bpy.ops.object.mode_set(mode='OBJECT')
                    bpy.data.objects[ob.name].select = True 
                    strFile += self.exportActionsForObject(ob)
            else:
                strFile += self.emitWarning("Object was hidden, it won't get exported: " + self.getObjectName(ob, None))
        return strFile
    def getParentString(self, ob):
        strParent = ""
        strParentType = "NONE"
        if ob.parent != None:
            strParentType = str(ob.parent_type)
            if ob.parent_type == "BONE":
                bone = ob.parent.data.bones[ob.parent_bone]
                strParent = self.getObjectName(ob.parent, bone)
            else:
                strParent = self.getObjectName(ob.parent, None)
        return "\"" + strParent + "\" \"" + strParentType + "\""
    #Export .MOD file
    def getMeshPartMODFileDescBeg(self, ob):
        #this is the MPT mod file part in the OBJ fie.
        strFile = ""
    
        #Mesh Parts
        #if ob.name.endswith(self.getContactBoxNameSuffix()):
        #    #Contact Box - Export the min/max of the meshbox.   
        #    strFile += "cbx " + self.strModelName + "." + self.getObjectName(ob, None) + " " + self.getBoxString(ob.data) + "\n\n"
        #elif ob.name.endswith(self.getPhysicsVolumeNameSuffix()):
        #    #Physics Volume - Export the min/max of the box.             
        #    strFile += "phyv " + self.strModelName + "." + self.getObjectName(ob, None) + " " + self.getBoxString(ob.data) + "\n\n"
        #else:
        #Regular Mesh
        strParent = self.getParentString(ob)

        #Part Name
        strFile += "mpt_beg \"" + self.getObjectName(ob, None) + "\" " + strParent + "\n"
        strFile += "mpt_hide_render " + str(ob.hide_render) + "\n"
                    
        if ob.rigid_body != None:
            print("Object has rigid body");
            strFile += "# collision shape, kinematic (t/f - if it follows an armature) dynamic (t/f - if it RESPONDS physically, otherwise it just regiters collisions) \n"
            strFile += "physics_shape \"" + str(ob.rigid_body.collision_shape) + "\" " + str(ob.rigid_body.kinematic) + " " + str(ob.rigid_body.enabled) + "\n"
        
        #Matrices
        
        cur_mode = bpy.context.object.mode
        bpy.ops.object.mode_set(mode='EDIT')
        strFile += "#mpt_world " + self.matToString(self.glMat4(ob.matrix_world)) + "\n"
        strFile += "#mpt_local " + self.matToString(self.glMat4(ob.matrix_local)) + "\n"
        strFile += "mpt_basis " + self.matToString(self.glMat4(ob.matrix_basis)) + "\n"
        strFile += "#location " + self.vec3ToString(self.glVec3(ob.location)) + "\n"
        strFile += "mpt_parent_inverse " + self.matToString(self.glMat4(self.getParentInverse(ob))) + "\n"
        bpy.ops.object.mode_set(mode=cur_mode)
        #strFile += "mpt_local " + self.matToString(self.glMat4(ob.matrix_local)) + "\n"
        #strFile += "mpt_basis " + self.matToString(self.glMat4(ob.matrix_basis)) + "\n"
        
        #Export Material
        strFile += self.printMaterialForMpt(ob)

        return strFile
    def printMaterialForMpt(self, ob):
        strFile = ""
        if ob.material_slots != None:
            for mat_slot in ob.material_slots:

                
                bDiffuseTex = False

                if mat_slot != None:
                    if mat_slot.material != None:
                        strFile += "mat_beg \"" + self.getObjectName(ob, None) + "\"\n";
                        for mtex_slot in mat_slot.material.texture_slots:
                            if mtex_slot != None:
                                if mtex_slot.texture != None:
                                    if hasattr(mtex_slot.texture , 'image'):
                                        if mtex_slot.texture.image != None:
                                            img = mtex_slot.texture.image
                                            if img.file_format == "PNG" or img.file_format == "TARGA" or img.file_format == "TARGA_RAW":
                                                #fileName = os.path.splitext(os.path.basename(img.filepath))[0]
                                                #fileExt = os.path.splitext(os.path.basename(img.filepath))[1]
                                                filename = os.path.basename(img.filepath)
                                                #These values are taken from the INFLUENCE dropdown on the texture panel of the object
                                                if len(os.path.basename(img.filepath)) > 0:
                                                    if mtex_slot.use_map_color_diffuse:
                                                        #Note: use_map_alpha doesn't matter.  we enable alpha if 'Transparency' is enabled on the model.
                                                        strFile += "mat_tex_diffuse \"" + filename + "\" " + str(mtex_slot.diffuse_color_factor) + " " + str(mtex_slot.alpha_factor) + "\n"
                                                        bDiffuseTex = True
                                                    if mtex_slot.use_map_normal:
                                                        strFile += "mat_tex_normal \"" + filename + "\" " + str(mtex_slot.normal_factor) + "\n"
                                                
                                                
                                                if self.blnSaveTextures == True:
                                                    self.saveImagePng(img)
                                                    strFile += "#saved to " + os.path.join(self.getMobFullPathWithoutFilename() , os.path.basename(img.filepath)) + "\n"
                                                
                                            else:
                                                strErr = "#File Format " + img.file_format + " not supported, or image not found.\n"
                                                print(strErr)
                                                strFile+= strErr
                
                        #Uif diffuse texture isn't supplied we need to render in the color.
                        strFile += "mat_diffuse " + self.color3ToString(mat_slot.material.diffuse_color) + " " + str(mat_slot.material.diffuse_intensity) +"\n"                                        
                        strFile += "mat_spec " + self.color3ToString(mat_slot.material.specular_color) + " " + str(mat_slot.material.specular_intensity) + " " + str(mat_slot.material.specular_hardness) + "\n"
                        if mat_slot.material.raytrace_mirror.use:
                            strFile += "mat_mirror " + self.color3ToString(mat_slot.material.mirror_color) + " " + str(mat_slot.material.raytrace_mirror.reflect_factor) + "\n"
                        #So - to support transparency - Under "Transparency" of the material
                        #Alpha - used, IOR - used, Filter - used (for glass color)
                        #the followign must be set to equal blender
                        #Fresnel - 0.0, Falloff - 1.0, Limit - 0.0, Depth - 1, Amount - 1.0
                        if mat_slot.material.use_transparency:
                            strFile += "mat_transparency " + str(mat_slot.material.alpha) + " " + str(mat_slot.material.raytrace_transparency.ior) +  " " + str(mat_slot.material.raytrace_transparency.filter) + "\n"
                                
                        strFile += "mat_end \"" + self.getObjectName(ob, None) + "\"\n";    
            return strFile
    def saveImagePng(self, img):
        #image = bpy.data.images.new("Test", alpha=True, width=16, height=16)
        #image.use_alpha = True
        #image.alpha_mode = 'STRAIGHT'
        tmp1 = img.filepath_raw
        filename = os.path.join(self.getMobFullPathWithoutFilename() , os.path.basename(img.filepath))
        print("Full image path: " + filename)
        img.filepath_raw = filename
        
        tmp2 = img.file_format 
        img.file_format = 'PNG'
        try:
            img.save()
        except:
            print("Failed to save image " + filename)
        
        img.filepath_raw = tmp1
        img.file_format = tmp2
        return
    def getMeshPartMODFileDescEnd(self, ob):
        #this is the MPT mod file part in the OBJ fie.
        strFile = ""
        strFile += "mpt_end \"" + self.strModelName + "." + self.getObjectName(ob, None) + "\"\n"
        
        return strFile;
    #Export .OBJ file
    def exportAllAsMob(self, path):
    
        #Set mode back to object to get the animated bones
        strFile = ""

        strFile += "# Blender MOD/OBJ/SKN (MOB) file\n"
        strFile += "# MetalMario971\n"
        strFile += "# 20160507\n"
        
        strFile += "mod_beg \"" + self.strModelName + "\" \"" + str(self.fVersion) + "\"\n"
        strFile += "\n"
        #strFile += "frr " + str(bpy.context.scene.render.fps) + " #Frame rate\n"
        strFile += "# Base Fps = " + str(int(bpy.context.scene.render.fps)) + " Grain = " + str(int(self.intGranularity)) + "\n"
        strFile += "frr " + str(int(bpy.context.scene.render.fps) * int(self.intGranularity)) +  " #Frame rate\n"
        
        #combining all into one file.
        strFile += self.exportAllActionNames()
        strFile += self.exportAllObjectNames()
        #strFile += self.exportWorldStuff()
        strFile += self.exportAllArmatures()
        strFile += self.exportAllKeyframes()
        
        bpy.context.scene.frame_set(0, 0)    
        strFile += self.exportAllMeshes()

        strFile += "mod_end \"" + self.strModelName + "\"\n"
        
        #print for debug, and write to file
        #print(strFile)

        with open(path, 'w') as f:
            f.write(strFile)
            
        return

    def mesh_triangulate(me):
        import bmesh
        bm = bmesh.new()
        bm.from_mesh(me)
        bmesh.ops.triangulate(bm, faces=bm.faces)
        bm.to_mesh(me)
        bm.free()        
    def exportAllMeshes(self):
        bpy.ops.object.mode_set(mode='OBJECT')
        
        
        strFile = ""
        #strFile += "skn " + sknName + "\n"
        #  strFile += "beh " + getattr(self,"eAnimBehavior") + "\n"        
        #strFile += "#If obj file is not specified per MPT then it is used for every model that doesn't have an obj directive.\n"
        #strFile += "obj " + objName + "\n"
       # strFile += "\n"    
        #strFile += "# Blender OBJ file\n"
        #strFile += "# Derek Page\n"
        #strFile += "# 20160507\n"
        
        #Count objects and print to file
       # iObjectCount = 0
       # for ob in bpy.data.objects:
       #     if ob.type != 'MESH':
       #         continue
       #         iObjectCount+=1
       #         
       # strFile += "# " + str(iObjectCount) + " Meshes Exported\n"

        #relative vertex offset for the current mesh.
        iMeshVertexOffset = 0
        iMeshNormalOffset = 0
        iMeshTCoordOffset = 0

        for ob_mesh in bpy.data.objects:
            if ob_mesh.type != 'MESH':
                continue
            
            if ob_mesh.hide == True:
                strFile += self.emitWarning("Object was hidden, it won't get exported: " + self.getObjectName(ob_mesh, None))
                continue
            
            #if self.isSpecialMesh(ob_mesh):
            #    continue

            context.scene.objects.active = ob_mesh
           
            print("Exporting Mesh.." + self.getObjectName(ob_mesh, None) + " type = " + str(ob_mesh.type))

            
            
            ###################################   
            # STEP 1
            #triangulate mesh, and other operations
            #NEW 2017 12 28 - triangulate the mesh
            bm_new = bmesh.new()
            bm_old = bmesh.new()
            bm_new.from_mesh(ob_mesh.data)
            bm_old.from_mesh(ob_mesh.data)
            bmesh.ops.triangulate(bm_new, faces=bm_new.faces[:], quad_method=0, ngon_method=0)
            
            #redo normals
            #bmesh.ops.recalc_face_normals(bm, faces=bm.faces)
            #bmesh.ops.recalc_face_normals(bm_new, faces=bm_new.faces[:])
            #bm_new.normal_update()
            bm_new.to_mesh(ob_mesh.data)
            bm_new.free()

            #bpy.types.Scene.update()
            #####################
            # STEP 1.1 
            # Apply transform
            
            
            ###################################   
            # STEP 2
            strFile += self.getMeshPartMODFileDescBeg(ob_mesh);
            strFile += "# " + self.getObjectName(ob_mesh, None) + " incoming...\n"
            
            iMeshVertexCount = 0
            iMeshNormalCount = 0
            iMeshTCoordCount = 0
    
            bHasTCoords = False
            
            ##############################################
            #VERTS
            strFile += "# " + str(len(ob_mesh.data.vertices)) + " VERTEXES \n"
            #strFile += "v_beg " + str(len(ob_mesh.data.vertices)) + "\n"
            for vert in ob_mesh.data.vertices:
                vTransform = vert.co
                #v2 = ob_mesh.matrix_world * vert.co
                strFile += "v " + self.vec3ToString(self.glVec3(vTransform)) + "\n"
                #strFile += "#vtx " + self.vec3ToString(self.glVec3(v2)) + "\n"
                iMeshVertexCount += 1
            #strFile += "v_end " + str(len(ob_mesh.data.vertices)) + "\n"
               
            ##############################################
            #WEIGHTS
            strFile += self.getVertexWeightsForMesh(ob_mesh)
            
            ##############################################
            #TEXTURES
            uv_dict = {}
            uv_face_mapping = [None] * len(ob_mesh.data.polygons) #store a map of each face index to the given tex coord indexes ordered by vertex index

            strTcoords = ""
            #iLocalTCoordIndex =0
            bEmitted = False
            for f in ob_mesh.data.polygons: 
                uv_face_mapping[f.index] = []
                for loop_index in f.loop_indices:
                    if ob_mesh.data.uv_layers is not None:
                    
                        if ob_mesh.data.uv_layers.active is not None:
                            if ob_mesh.data.uv_layers.active.data is not None:
                                bHasTCoords = True
                                cur_uv = ob_mesh.data.uv_layers.active.data[loop_index].uv
                                
                                #create a dictionary key (python-wise it's just a comma list)
                                key = ob_mesh.data.loops[loop_index].vertex_index, self.veckey2d(ob_mesh.data.uv_layers.active.data[loop_index].uv)

                                if uv_dict.get(key) is None:
                                    strTcoords += "vt " + self.vec2ToString(cur_uv) + "\n"
                                    #store index to the current tcoord with a key.
                                    uv_dict[key] = len(uv_dict)
                                #store the face mapping.
                                uv_face_mapping[f.index].append( uv_dict.get(key) )
                            else:             
                                self.emitError(ob_mesh.name + ": Active UV layer had no data");                                    
                        else:             
                            if bEmitted == False:
                                self.emitError(ob_mesh.name + ": One Or More Faces (or All) did not have any ACTIVE UV layers");                                   
                                bEmitted = True
                    else:             
                        self.emitError(ob_mesh.name + ": Model did not have any UV layers");        
                     
            iMeshTCoordCount += len(uv_dict);
            strFile += "# " + str(len(uv_dict)) + " TCOORDS \n"   
            strFile += strTcoords

            del uv_dict
            
            ##############################################
            #NORMALS
            #We need to use a dict for face normals BECAUSE to keep the data footprint minimal, also it follows .obj format
            #Basically do the same here as we did with the TCoords, store a dict of x,y,z -> index and reference it on the face
            norm_dict = {} #map nx, ny, nz -> normal index (simply the position in tehe dictionary)
            #normal_face_mapping = [None] * len(ob_mesh.data.polygons)
            normal_face_mapping = [0] * len(ob_mesh.data.loops)
            strNormals = ""
            for f in ob_mesh.data.polygons:
                for iInd, loop_index in enumerate(f.loop_indices):
                    v_or_f_norm = None

                    #v_or_f_norm = ob_mesh.data.loops[loop_index].normal #-- this is what the blender obj exporter uses and it's 0,0,0 for some reason?
                    
                    #if we're a smooth face use the face normal.  blender doesn't have custom normals AFAIK (kinda lame)
                    if f.use_smooth == True:
                        v_or_f_norm = ob_mesh.data.vertices[f.vertices[iInd]].normal
                    else:
                        v_or_f_norm = f.normal
                    
                    norm_key = self.veckey3d(v_or_f_norm)#create key for dict
                    if norm_dict.get(norm_key) is None:
                        norm_dict[norm_key] = len(norm_dict)
                        strNormals += "vn " + self.vec3ToString(self.glVec3(v_or_f_norm)) + "\n"
                    normal_face_mapping[loop_index] = norm_dict.get(norm_key)

            iMeshNormalCount += len(norm_dict)
            strFile += "# " + str(len(norm_dict)) + " NORMALS \n"   
            strFile += strNormals

            del norm_dict
            
            ##############################################
            #FACES
            strFile += "# " + str(len(ob_mesh.data.polygons)) + " FACES \n"
            
            strFile += "# Mesh V Count: " + str(iMeshVertexCount) + ", File Total:" + str(iMeshVertexOffset + iMeshVertexCount) + "\n"
            strFile += "# Mesh T Count: " + str(iMeshTCoordCount) + ", File Total:" + str(iMeshTCoordOffset + iMeshTCoordCount) + "\n"
            strFile += "# Mesh N Count: " + str(iMeshNormalCount) + ", File Total:" + str(iMeshNormalOffset + iMeshNormalCount) + "\n"
            
            strFile += "g \"" + self.getObjectName(ob_mesh, None) + "\"\n"
            
            
            #strFile += "f_beg " + str(len(ob_mesh.data.polygons)) + "\n"
            if self.blnFlipTris == True:
                strFile+="# Note: Winding is flipped by the CW engine - not exported\n"
                strFile += "mpt_flip_tris True\n"
            else:
                strFile += "mpt_flip_tris False\n"

            if self.blnCalcNormals == True:
                strFile += "mpt_calc_normals True\n"
            else:
                strFile += "mpt_calc_normals False\n"
                
            if self.blnFlipUVs == True:
                strFile+="# Note: UV is flipped by the CW engine - not exported\n"
                strFile += "mpt_swap_uvs True\n"
            else:
                strFile += "mpt_swap_uvs False\n"
            
            #Smoothing isn't working for blender for some reason
            strFile += "s off\n"            
            for f in ob_mesh.data.polygons:     
                if f.use_smooth == False:
                    strFile+= self.exportFace(f, iMeshVertexOffset, iMeshNormalOffset, iMeshTCoordOffset, uv_face_mapping, normal_face_mapping, bHasTCoords);
    
            strFile += "s 1\n"
            for f in ob_mesh.data.polygons:     
                if f.use_smooth == True:
                    strFile+= self.exportFace(f, iMeshVertexOffset, iMeshNormalOffset, iMeshTCoordOffset, uv_face_mapping, normal_face_mapping, bHasTCoords);
 
            #strFile += "f_end " + str(len(ob_mesh.data.polygons)) + "\n"
            
            iMeshVertexOffset += iMeshVertexCount
            iMeshNormalOffset += iMeshNormalCount
            iMeshTCoordOffset += iMeshTCoordCount

            ###################################   
            # STEP 3
            #end using the created bmesh and return the old mesh back to our copied mesh.
            bm_old.to_mesh(ob_mesh.data)
            bm_old.free()

            strFile += self.getMeshPartMODFileDescEnd(ob_mesh)
            
        return strFile
    def exportFace(self, f, iMeshVertexOffset, iMeshNormalOffset, iMeshTCoordOffset, uv_face_mapping, normal_face_mapping, bHasTCoords):
        strFile = ""
        #create list of indices first. if we have quads then we have to triangulate.
        indices = []
        for iInd, loop_index in enumerate(f.loop_indices):
            #indexes index ALL VERTS IN THE FILE BEFORE.  so we have to add an offset to them.
            v_ind = f.vertices[iInd] + iMeshVertexOffset + 1  #All OBJ files start at index 1
            n_ind = normal_face_mapping[loop_index] + iMeshNormalOffset + 1
            
            if bHasTCoords==True:
                t_ind = uv_face_mapping[f.index][iInd] + iMeshTCoordOffset + 1
                strIndex = "%d" % (v_ind) + "/" + "%d" % (t_ind)  + "/" + "%d" % (n_ind)
            else:
                strIndex = "%d" % (v_ind) + "//" + "%d" % (n_ind)
            indices.append(strIndex)

        #2017 12 28 now that we triangulate the mesh, we shouldn't be doing quads
        if len(indices) == 3:
            #acording to doc loop_indices is equivalent to range(p.loop_start, p.loop_start + p.loop_total):
            ind0 = indices[0]
            ind1 = indices[1]
            ind2 = indices[2]
            
            strFile += "f "
            strFile += ind0 + " "
            strFile += ind1 + " "
            strFile += ind2 + "\n"                    
        else:
            raise Exception('Error - Too many indices "' + str(len(indices)) + '" for face vertex in mesh "'+ob_mesh.name+'". Consider triangulating mesh (edit mode, select faces, CTRL+T).')
        return strFile
    def veckey2d(self, v):
        return round(v[0], 4), round(v[1], 4)
    def veckey3d(self, v):
        return round(v.x, 4), round(v.y, 4), round(v.z, 4)

    #Get the list of child bones form the given bone oject
    #    obj - the given Scene object
    #    parentBone - the given parent bone.
    #    NOTE: blender doesn't store parent/child bones automaticalyl.  cdum
    def getBoneChildren(self, parentBone, boneList):
        #obj = self.id_data
        children = []
        for iBone in boneList:
            if not iBone or not parentBone or not iBone.parent:
                continue
            if iBone.parent.name == parentBone.name:
                children.append(iBone)
                
        return children
    def getAllBones(self, ob, boneList):
        strBones = ""
        strBones += "bones_beg\n"
        for pose_bone in boneList:
            strBones += self.getBoneString(ob, pose_bone, boneList)
        strBones += "bones_end\n"
        return strBones
        
    def getBoneString(self, ob_arm, in_pose_bone, boneList):
        ################################################################
        #from the .x exportrer
        ## BoneMatrix transforms mesh vertices into the
        ## space of the bone.
        ## Here are the final transformations in order:
        ##  - Object Space to World Space
        ##  - World Space to Armature Space
        ##  - Armature Space to Bone Space
        ## This way, when BoneMatrix is transformed by the bone's
        ## Frame matrix, the vertices will be in their final world
        ## position.
        #
        #self.BoneMatrix = ArmatureObject.data.bones[BoneName] \
        #    .matrix_local.inverted()
        #self.BoneMatrix *= ArmatureObject.matrix_world.inverted()
        #self.BoneMatrix *= BlenderObject.matrix_world
        ################################################################


        strBone = "bone_beg " +  "\"" + self.getObjectName(ob_arm, in_pose_bone) + "\" " + str(self.getBoneId(in_pose_bone.name, boneList)) + " "
        if not in_pose_bone.parent:
            strBone += "\"\" \"NONE\" \n" # No parent, root
        else:
            strBone += "\"" + self.getObjectName(ob_arm, in_pose_bone.parent) + "\" \"NONE\" \n"
        
        strBone += "bone_head " + self.vec3ToString(self.glVec3(in_pose_bone.head)) + "\n"
        strBone += "bone_tail " + self.vec3ToString(self.glVec3(in_pose_bone.tail)) + "\n"

        
        # bpy.ops.object.mode_set(mode='EDIT')
        #editBoneList = ob_arm.data.edit_bones
        # strFile += "# INVERSE BIND POSE MATRICES INCOMING (BIND_POSES)\n"
        # strFile += "bp_beg " + str(len(editBoneList)) + " \n"   
        
        #bindworld = self.matrix_world(ob_arm, in_bone.name)
        #bindworld = in_pose_bone.bone.matrix_local
        #if in_pose_bone.parent:
        #    bindworld = in_pose_bone.bone.parent.matrix_local.inverted() * in_pose_bone.bone.matrix_local
            
        #else:
        #    #no parent, try armature i guess
        #    bindworld = ob_arm.matrix_local.inverted() * in_pose_bone.bone.matrix_local
        #strBone += "#debug: basis mat before gl conv = " + str(in_bone.matrix_basis) + "\n"
        #strBone += "#debug: basis mat before gl conv = " + self.matToString(bindworld) + "\n"
        #https://blender.stackexchange.com/questions/44637/how-can-i-manually-calculate-bpy-types-posebone-matrix-using-blenders-python-ap
        
        #strBone += "bone_bind_world " +  "m44 " + self.getKeyframeDataMat4(bindworld) + "\n";
        #strBone += "bone_bind_world_inv " +  "m44 " + self.getKeyframeDataMat4(in_pose_bone.bone.matrix_local.copy().inverted()) + "\n";
        #DEBUG
        #self.dump(in_pose_bone)
        #self.dump(ob_arm)
        #2017 12 16 - this made no sense.  why not just use the bind?
        # 2AM - because  blender bind mat makes no sense, it's all wrong basis
        for iBone in boneList:  
            if iBone.parent == None:
                strBone += self.getPoseBoneBindMatrices(iBone, Vector([0,0,0]), boneList, in_pose_bone.name) 
        
        strBone += "bone_end " +  "\"" + self.getObjectName(ob_arm, in_pose_bone) + "\"\n"
        
        
        return strBone 
    #Create path for file if it doesn't exist
    def makePath(self, filePath):
        strPath = os.path.dirname(os.path.abspath(filePath.strip()))
        if not os.path.exists(strPath):
            os.mkdir(strPath)
        return
    #def getNumAnimations(self):
    #    return self.iMaxAnimations
    def getBoneId(self, strName, objBoneList):
        #Note: this could prove problematic in the future.
        #Assuming the ordinal position of the bone is it's bone ID referenced in the
        #Group in the Vertex Weight

        i=0
        for bone in objBoneList:
            if bone.name == strName:
                return i
            i+=1
        #-1 is an allowed value for some vgroups if you have multiple armatures on a single mesh
        return -1
    def tryBackupFile(self):
        
        if self.blnMakeBackup == False:
            return
        
        fullPath = self.getMobFullPathWithoutFilename()
        #This will remove the folder from the  path
        strPath = os.path.dirname(os.path.abspath(fullPath.strip())) 
        strBackupPath = os.path.join(strPath, "bp_backup")
        
        if not os.path.exists(strBackupPath):
            os.mkdir(strBackupPath)
        
        filePath = self.getMobFullPathWithFilename()
        strFileName, strFileExt = os.path.splitext(os.path.basename(filePath))        
        strBackupFilePath = os.path.join(strBackupPath, strFileName + "_" + str(time.time()) + strFileExt)

        
        #copy src, dst
        if os.path.isfile(filePath):
            shutil.copyfile(filePath, strBackupFilePath)
            
        print("Backup success: " + strBackupFilePath)
        
        return
    def exportActionsForObject(self, ob):
        #New export which exports actions
        strFile = ""

        #AnimData (struct)
        #https://docs.blender.org/api/blender_python_api_2_62_release/bpy.types.AnimData.html
        if ob.animation_data == None:
            strFile += "#" + self.getObjectName(ob, None) + " - No animation data\n"
            return strFile

        strFile += "#" + self.getObjectName(ob, None) + " has animation data " + str(len(ob.animation_data.nla_tracks)) + " NLA Tracks\n"
        for nla in ob.animation_data.nla_tracks:
            nla.select = True
            strFile += "# NLA STrips: " + str(len(nla.strips)) + "\n"
            for strip in nla.strips:
                curAction = strip.action
                strFile += "#Action Found : " + curAction.name + "\n"
                #print name
                strFile += "\n"
                
                #keyrames
                keyframes = []

                iMinKeyf = self.getMinKeyframeForAction(curAction)
                iMaxKeyf = self.getMaxKeyframeForAction(curAction)
                
                #dump keyframes to file.
                if iMinKeyf < iMaxKeyf == 0:
                    strFile += "#Animation had no keyframes.\n"
                else:
                    strFile += self.printKeyframeBlockForSelectedObject(ob, iMinKeyf, iMaxKeyf, curAction)

        return strFile 
    def getMinKeyframeForAction(self, curAction):
        iRet = 9999999
        for fcu in curAction.fcurves:
            for keyf in fcu.keyframe_points:
                x, y = keyf.co
                if x < iRet:
                    iRet = x
        return int(iRet)
    def getMaxKeyframeForAction(self, curAction):        
        iRet = -9999999
        for fcu in curAction.fcurves:
            for keyf in fcu.keyframe_points:
                x, y = keyf.co
                if x > iRet:
                    iRet = x
        return int(iRet)    

    def getActionKeyframeDataForKeyframeIndex(self, pAction, iIndex):
        for fcu in pAction.fcurves:
            #strFile += "DEBUG: data_path '" + str(fcu.data_path) +
            #"' channel '" + str(fcu.array_index) + "' driver '" + str(fcu.driver) + "'\n"
            for keyf in fcu.keyframe_points:
                #x, y = keyf.co
                x, y = keyf.co
                ind = math.ceil(x)
                if iIndex == ind:
                    return keyf
                    
        return None
    #get the keyframe string for selected keyframe
    def printKeyframeBlockForSelectedObject(self, ob, iMinKey, iMaxKey, curAction):
        strKeyframe = ""
        if ob.type == 'ARMATURE':
            for pose_bone in ob.pose.bones:
                strKeyframe += "act_beg \"" + self.getObjectName(curAction, None) + "\" \"" + self.getObjectName(ob, pose_bone) + "\"\n"
                strKeyframe += self.printKeyframeBlockForPoseBone(ob, pose_bone, iMinKey, iMaxKey, curAction)
                strKeyframe += "act_end \"" + self.getObjectName(curAction, None) + "\" \"" + self.getObjectName(ob, pose_bone) + "\"\n\n"
        #Mesh only here, but obviously we can do this for ANY object (armature, etc)
        elif ob.type == 'MESH':
            strKeyframe += "act_beg \"" + self.getObjectName(curAction, None) + "\" \"" + self.getObjectName(ob, None) + "\"\n"        
            strKeyframe += self.printKeyframeBlockForObject(ob, iMinKey, iMaxKey, curAction)
            strKeyframe += "act_end \"" + self.getObjectName(curAction, None) + "\" \"" + self.getObjectName(ob, None) + "\"\n\n"
        return strKeyframe
    def printKeyframeBlockForObject(self, ob_mesh, iMinKey, iMaxKey, curAction):
        strKeyframe = ""
        #set the action.
        ob_mesh.animation_data.action = curAction
        #strKeyframe += "kf_beg \"" + self.getObjectName(ob_mesh, None) + "\" " + str(len(keyframes)) + "\n"
        iGrainKey = 0
        for iKeyFrame in range(iMinKey, iMaxKey+1):
            #this little block gets the final keyframe
            iGrainMax = self.intGranularity
            if iKeyFrame == iMaxKey:
                iGrainMax = 1
            for iGrain in range(0, iGrainMax):
                fGrain = float(iGrain) / float(iGrainMax)
                bpy.context.scene.frame_set(iKeyFrame, fGrain)    
                
                #*Matrix world here hopefully applies all parent transforms
                #20171228 - old object export
                #curMat = None
                #curMat = ob_mesh.matrix_world.to_4x4()
                
                #20171228 new relative export
                curMat = ob_mesh.matrix_basis.to_4x4();
                #if ob_mesh.parent != None:
                #    curMat = ob_mesh.parent.matrix_basis.to_4x4().inverted() * curMat  
                #curMat = ob_mesh.matrix_world.to_4x4()
                #if ob_mesh.parent != None:
                #    curMat = ob_mesh.parent.matrix_world.to_4x4().inverted() * curMat  
                


                #curMat = (ob_mesh.parent.matrix_local.inverted() * ob_mesh.matrix_local) * ob_mesh.matrix_basis
                #else:
                    #curMat = ob_mesh.matrix_local * ob_mesh.matrix_basis
                    
                #strKeyframe += self.debugDumpMatrix("curMat", curMat)
                #strKeyframe += self.debugDumpMatrix("ob_mesh.matrix_basis", ob_mesh.matrix_basis)
                #strKeyframe += self.debugDumpMatrix("ob_mesh.matrix_world.to_4x4()", ob_mesh.matrix_world.to_4x4())

                strKeyframe += self.printKeyframeMatrixForAction(iGrainKey, curMat, curAction)
                iGrainKey+=1
        #strKeyframe += "kf_end \"" + self.getObjectName(ob, None) + "\"\n"
        return strKeyframe
    def printKeyframeBlockForPoseBone(self, ob_arm, pose_bone, iMinKey, iMaxKey, curAction):
        strKeyframe = ""
        #set the action.
        
        ob_arm.animation_data.action = curAction
        iGrainKey = 0
        #strKeyframe += "kf_beg \"" + self.getObjectName(ob_arm, pose_bone) + "\" " + str(len(keyframes)) + "\n"
        #for iKeyFrame in range(bpy.context.scene.frame_start, bpy.context.scene.frame_end+1):
        for iKeyFrame in range(iMinKey, iMaxKey+1):
            #this little block gets the final keyframe
            iGrainMax = self.intGranularity 
            if iKeyFrame == iMaxKey:
                iGrainMax = 1        
            for iGrain in range(0, iGrainMax):
                fGrain = float(iGrain) / float(iGrainMax) 
                bpy.context.scene.frame_set(iKeyFrame, fGrain)

                # parentMatrix = Matrix.Identity(4)
                # #we werwe using the channel matrix, I think we want the actual final driver matrix.
                # #this is the exact code from https://blender.stackexchange.com/questions/15170/blender-python-exporting-bone-matrices-for-animation-relative-to-parent
                
                #20171218 debugging shows matrix-basis to contain the actual information we need.
                #This contains only rotation as far as I can tell
                #curMatrix = pose_bone.matrix_channel
                
                parentMatrix = Matrix.Identity(4)

                curParent = pose_bone.parent
                curMatrix = pose_bone.matrix_channel.to_3x3().to_4x4()
                if curParent != None:
                    curMatrix = curParent.matrix_channel.to_3x3().to_4x4().inverted() * curMatrix  #effectively removes translation component

                
                #https://blender.stackexchange.com/questions/35125/what-is-matrix-basis
                #It's also worth mentioning it's role in other matrices: matrix_local =
                #  matrix_parent_inverse * matrix_basis, and matrix_world = parent.matrix_world * matrix_local. – misnomer Dec 10 '16 
                
                #parentMat = cbm = Matrix.Identity(4)
                #if pose_bone.parent != None:
                #    parentMat = pose_bone.parent.matrix.inverted() # * pose_bone.matrix  #effectively removes translation component
                #"chartest_noa.Punch" "chartest_noa.Armature.BoneShoulderR
                #strKeyframe += self.debugDumpMatrix("pose_bone.bone.matrix_local.to_4x4()", pose_bone.bone.matrix_local.to_4x4())
                #strKeyframe += self.debugDumpMatrix("pose_bone.bone.matrix", pose_bone.bone.matrix.to_4x4())
                #strKeyframe += self.debugDumpMatrix("pose_bone.matrix_basis", pose_bone.matrix_basis)
                #strKeyframe += self.debugDumpMatrix("pose_bone.matrix_channel", pose_bone.matrix_channel)
                #strKeyframe += self.debugDumpMatrix("pose_bone.matrix", pose_bone.matrix)
                
                #strKeyframe += self.debugDumpMatrix("pose_bone.parent.matrix.inverted() * pose_bone.matrix", (parentMat.inverted() * pose_bone.matrix) * pose_bone.bone.matrix_local.to_4x4());
                #strKeyframe += self.debugDumpMatrix("pose_bone.parent.matrix.inverted() * pose_bone.matrix_basis", parentMat.inverted() * pose_bone.matrix_basis);
                #strKeyframe += self.debugDumpMatrix("pose_bone.bone.matrix_local.to_4x4() * pose_bone.matrix_basis",  pose_bone.matrix_basis * pose_bone.bone.matrix_local.to_4x4());
                #It looks like matrix_local is changing the basis of matrix_bind.  Matrix_bind has the correct rotation.
                
                #strKeyframe += self.debugDumpMatrix("pose_bone.parent.matrix.inverted() * pose_bone.matrix", curMatrix);
                #strKeyframe += self.debugDumpMatrix("self.glMat4(pose_bone.parent.matrix.inverted() * pose_bone.matrix)", self.glMat4(curMatrix));
                
             #   curParent = pose_bone.parent
             #   curMatrix = pose_bone.matrix_channel.to_3x3().to_4x4()
             #   if curParent != None:
             #       curMatrix = curParent.matrix_channel.to_3x3().to_4x4().inverted() * curMatrix  #effectively removes translation component
                #skinMat = curMatrix 

                strKeyframe += self.printKeyframeMatrixForAction(iGrainKey, curMatrix, curAction);
                iGrainKey+=1
            
        #strKeyframe += "kf_end \"" + self.getObjectName(ob_arm, pose_bone) + "\"\n"
        return strKeyframe
    def debugDumpMatrix(self, str, in_matrix):
        #return ""
        strDebug = ""
        loc, rot, sca = in_matrix.decompose()
        strDebug += "\n\n"
        strDebug += "#" + str + " mat\n" + self.matToString(in_matrix.to_4x4(), ",", True) + "\n"
        strDebug += "#    loc       (" + self.vec3ToString(loc) + ")\n"
        strDebug += "#    quat      (" + self.vec4ToString(rot) + ")\n"

        strDebug += "#    gl_quat:  (" + self.vec4ToString(self.glQuat(rot)) + ")\n"
        strDebug += "#to_euler_deg: (" + self.vec3ToString(self.euler3ToDeg(rot.to_euler("XYZ"))) + ")\n" 
        strDebug += "#gl_euler_deg: (" + self.vec3ToString(self.euler3ToDeg(self.glEuler3(rot.to_euler()))) + ")\n" 
        
        strDebug += "#AxAng(Blender) " + self.vec3ToString(self.glVec3(in_matrix.to_quaternion().axis)) 
        strDebug += "," + self.fPrec() % ((180.0)*in_matrix.to_quaternion().angle/3.14159)
        strDebug += "\n"
        
        strDebug += "#Ax,Ang(conv)   " + self.vec3ToString(self.glVec3(self.glMat4(in_matrix).to_quaternion().axis)) 
        strDebug += "," + self.fPrec() % ((180.0)*self.glMat4(in_matrix).to_quaternion().angle/3.14159)
        strDebug += "\n"       
                
        return strDebug
    def printKeyframeMatrixForAction(self, iKeyFrame, in_matrix, curAction):
        strKeyframe = ""
        #Matrix Data
        #loc, rot, sca = in_matrix.decompose()
        
        strKeyframe += "kf " + self.getKeyframeData(in_matrix, iKeyFrame) + "\n"
            
        return strKeyframe
    def getKeyframeData(self, in_matrix, iKeyFrame):
        strKeyframe = ""
        strFrame = ""
        if iKeyFrame >=0:
            strFrame = str(iKeyFrame) 
            
        if self.blnPRSKeyframes == True:
            strKeyframe = "prs " + strFrame + " " + self.getKeyframeDataPRS(in_matrix);
        else: 
            strKeyframe = "m44 " + strFrame + " " + self.getKeyframeDataMat4(in_matrix);
        
        return strKeyframe
    def getKeyframeDataMat4(self, in_matrix):
        strKeyframe = self.matToString(self.glMat4(in_matrix))
        return strKeyframe
    def getKeyframeDataPRS(self, in_matrix):
        #https://docs.blender.org/api/blender_python_api_2_78_release/mathutils.html
        
        loc, rot, sca = self.glMat4(in_matrix).transposed().decompose()
        #rot is a quaternion, but we need to flip the angles.

        strKeyframe = self.vec3ToString(loc, ",") + "," + self.vec4ToString(rot, ",") + "," + self.vec3ToString(sca, ",")
        return strKeyframe        
    
    # Export the v dubs.  VW [] [] []
    def getVertexWeightsForMeshArm(self, ob_mesh):
        strFile = ""
        bpy.ops.object.mode_set(mode='OBJECT')
      
        #for ob_mesh in bpy.data.objects:
        if ob_mesh.type != 'MESH':
            return "#No vertex weights for object (not mesh)\n";
        
        #if self.isSpecialMesh(ob_mesh):
        #    return "#No vertex weights for object (special mesh)\n";

        #group_names = [g.name for g in ob_mesh.vertex_groups]
        #group_names_tot = len(group_names)
        
        bWarn = False
        strFile += "#BEGIN VERTEX WEIGHTS: " + self.getObjectName(ob_mesh, None) + " " + str(len(ob_mesh.data.vertices)) + "\n"
        strFile += "#vw [arm count] 'arm name' bonecount boneid,weight 'arm name' bonecount boneid,weight ...\n"
        for iVert in ob_mesh.data.vertices:
            strVertWeights = ""
            iArmCount = 0
            for mod in ob_mesh.modifiers:
                if mod.type == 'ARMATURE':
                    ob_arm = mod.object
                    if ob_arm == None:
                        if bWarn == False:
                            strFile += self.emitWarning("Armature with no object found on mesh, no weight applied " + self.getObjectName(ob_mesh, None))
                            bWarn = True
                    else:
                        iArmCount += 1
                        
                        #We count groups because not all groups apply to this mesh vertex (multiple armatures for instance)
                        iGroupCount = 0
                        strWeightList = ""
                        
                        #Weight List
                        strApp = ""
                        for v_group in iVert.groups:
                            #Get the vertex group (the bone name) associated with this vertex
                            # Note - I'm not sure if v_group.group is always able to index into vertex_groups.
                            vgroup = ob_mesh.vertex_groups[v_group.group]
                            if vgroup.index != v_group.group:
                                strVertWeights += self.emitError("Index for group is invalid \"" + self.getObjectName(ob_arm, None) + "\"\"n")
                            
                            boneIndex = self.getBoneId(vgroup.name, ob_arm.pose.bones)
                            
                            if boneIndex > -1:
                                #For multiple armatures bound to a single mesh the index will be -1 - no bone found
                                #We just skip it - this is valid - because armature modifiers apply on the MESH level - but weights might be bound to each armature.
                                iGroupCount+=1
                                strWeightList +=  strApp + str(boneIndex) + "," + self.floatToString(v_group.weight)
                                strApp = ","
                        strWeightList += " "
 
                        strVertWeights +=  "" + str(self.getArmatureId(ob_arm)) + " " + str(iGroupCount) + " " + strWeightList
                
            strFile += "vw " +  str(iArmCount) + " " + strVertWeights + "\n"                 

        return strFile
    # Gets all bind pose matrixes as a string BP M44 [mat]
    def getPoseBoneBindMatrices(self, in_bone, parentBoneHead, boneList, forBoneName):
        strFile = ""
        worldPos = in_bone.head
        #print(in_bone.head)

        #Object Space Inverse Bind Pose
        bpoi =  Matrix.Translation(worldPos).copy().to_4x4()
        #bpoi.invert();

        #Joint Space Relative Bind Pose
        boneLocalTranslation = worldPos - parentBoneHead;
        bpjr = Matrix.Translation(boneLocalTranslation).copy().to_4x4()
        
        if in_bone.name == forBoneName:
            #keeping these as matrices (why change?)
            strFile += "bone_bind_world " +  "m44 " + self.getKeyframeDataMat4(bpoi) + "\n";
            #strFile += "bone_bind_local " +  "m44 " + self.getKeyframeDataMat4(bpjr) + "\n";
            #strFile += "bpoi " +  self.getKeyframeData(self.glMat4(bpoi)) + "\n"
            #strFile += "bpjr " +  self.getKeyframeData(self.glMat4(bpjr)) + "\n"
        
        boneChildren = self.getBoneChildren(in_bone, boneList)
        for child in boneChildren:            
            strFile += self.getPoseBoneBindMatrices(child, worldPos, boneList, forBoneName)
            
        return strFile
    #Convert matrix to 4x4 matrix, then to string
    def fPrec(self):
        return "%.8f"
    def matToString(self, mat, delim = ',', sp = False):
        strRet = ""
        mat_4 = mat.to_4x4()
        strApp = ""
        for row in range(4):
            if sp == True:
                strRet += "#"
            for col in range(4):
                #strRet += str(row) + " " + str(col)
                strFormat = "" + strApp + self.fPrec() + ""
                strRet += strFormat % mat_4[row][col] 
                strApp = delim
            if sp == True:
                strRet += "\n"
        return strRet
    def floatToString(self, float):
        strFormat = "" + self.fPrec() + ""
        strRet =  strFormat % (float)
        return strRet;
    def vec4ToString(self, vec4, delim=' '):
        strRet = ""
        strFormat = "" + self.fPrec() + delim + self.fPrec() + delim + self.fPrec() + delim + self.fPrec() + ""
        strRet =  strFormat % (vec4.x, vec4.y, vec4.z, vec4.w)
        return strRet      
    def vec3ToString(self, vec3, delim=' '):
        strRet = ""
        strFormat = "" + self.fPrec() + delim + self.fPrec() + delim + self.fPrec() + ""
        strRet =  strFormat % (vec3.x, vec3.y, vec3.z)
        return strRet      
    def color3ToString(self, vec3, delim=' '):
        strRet = ""
        strFormat = "" + self.fPrec() + delim + self.fPrec() + delim + self.fPrec() + ""
        strRet =  strFormat % (vec3.r, vec3.g, vec3.b)
        return strRet      
    def vec2ToString(self, vec2, delim=' '):
        strRet = ""
        strFormat = "" + self.fPrec() + delim + self.fPrec() + ""
        strRet =  strFormat % (vec2.x, vec2.y) 
        return strRet
    def glEuler3(self, eu):
        #NOTE: use Deep exploration to test- same coordinate system as vault
        #Convert Vec3 tgo OpenGL coords
        ret = Euler([eu.x, eu.y, eu.z])

        #-x,-z,-y is the correct export into deep expl
        #This is the correct OpenGL conversion
        tmp = ret.y
        ret.y = ret.z
        ret.z = tmp
        #ret.x = ret.x
        
        return ret
    def glQuat(self, quat):
        e = quat.to_euler()
        e = self.glEuler3(e)
        quat2 = e.to_quaternion()
        
        return quat2;
    def glVec3(self, vec):
        #NOTE: use Deep exploration to test- same coordinate system as vault
        #Convert Vec3 tgo OpenGL coords
        ret = Vector([vec.x, vec.y, vec.z])

        #-x,-z,-y is the correct export into deep expl
        #This is the correct OpenGL conversion
        tmp = ret.y
        ret.y = ret.z
        ret.z = tmp
        ret.x = ret.x
        
        return ret
    def glVec4(self, vec):
        ret = Vector([vec[0],vec[1],vec[2],vec[3]]);
        tmp = ret.y
        ret.y = ret.z
        ret.z = tmp
        ret.x = ret.x
        return ret;
    def glMat4(self, in_mat):
        #NOTE this functio works
       # global_matrix = io_utils.axis_conversion(to_forward="-Z", to_up="Y").to_4x4()
       # mat_conv = global_matrix * in_mat * global_matrix.inverted()
       # mat_conv = mat_conv.transposed()
       # return mat_conv
        
        #NOTE: t12/20/17 this actually works but seems to return a negative z value?
    
        #NOTE: use Deep exploration to test- same coordinate system as vault
        #convert matrix from Blender to OpenGL Coords
        m1 = in_mat.copy()
        m1 = m1.to_4x4()
        
        x=0
        y=1
        z=2
        w=3
        
        #change of basis matrix
        cbm = Matrix.Identity(4)
        cbm[x][0] = 1
        cbm[x][1] = 0
        cbm[x][2] = 0
        cbm[x][3] = 0
        
        cbm[y][0] = 0
        cbm[y][1] = 0
        cbm[y][2] = 1
        cbm[y][3] = 0
        
        cbm[z][0] = 0
        cbm[z][1] = 1
        cbm[z][2] = 0
        cbm[z][3] = 0
        
        cbm[w][0] = 0
        cbm[w][1] = 0
        cbm[w][2] = 0
        cbm[w][3] = 1
        
        #multiply CBM twice
        m1 = cbm.inverted() * m1 * cbm.inverted();
        
        #blender is row-major
        m1.transpose()  
        
        return m1
    def getContactBoxNameSuffix(self):
        return "_CB"
    def getPhysicsVolumeNameSuffix(self):
        return "_PV"        
    def getBoxString(self, data):
        #exports the bounding box of a mesh.
        strFile = ""
        flt_max = 10000000
        
        amin = Vector([flt_max,flt_max,flt_max])
        amax = Vector([-flt_max,-flt_max,-flt_max])
        
        for vert in data.vertices:
            if self.glVec3(vert.co).x < amin.x: amin.x = self.glVec3(vert.co).x
            if self.glVec3(vert.co).y < amin.y: amin.y = self.glVec3(vert.co).y
            if self.glVec3(vert.co).z < amin.z: amin.z = self.glVec3(vert.co).z
            if self.glVec3(vert.co).x > amax.x: amax.x = self.glVec3(vert.co).x
            if self.glVec3(vert.co).y > amax.y: amax.y = self.glVec3(vert.co).y
            if self.glVec3(vert.co).z > amax.z: amax.z = self.glVec3(vert.co).z
                
        #Note: amin, amax are already in OpenGL coordinates above.
        strFile += self.vec3ToString(amin) + " "
        strFile += self.vec3ToString(amax) + "\n"
        #strFile += "%.6f %.6f %.6f " % amin[:] + ""
        #strFile += "%.6f %.6f %.6f" % amax[:] + ""    
        
        return strFile
    def applyAllLocalTransforms(self):
        #bake all transforms for all objects
        for ob in bpy.data.objects:
            if ob.type != 'MESH':
                continue
                #Apply object transform
                context.scene.objects.active = ob
                ob.select = True
                bpy.ops.Object.transform_apply(location=True, rotation=True, scale=True)

    #def isSpecialMesh(self, ob):
    #    #The mesh is used by the vault system.  We do this based on mesh name.
    #    #Contact Boxes are skipped
    #    if ob.name.endswith(self.getContactBoxNameSuffix()) == True:
    #        return True
    #
    #    #Phy Vols are skipped
    #    if ob.name.endswith(self.getPhysicsVolumeNameSuffix()) == True:
    #        return True
    #            
    #    return False
    def getObjectName(self, ob, ob_bone):
        strName = ""
        strName += self.strModelName + "." + ob.name
        if ob_bone != None:
            strName += "." + ob_bone.name
        return strName
    def emitWarning(self, strMsg):
        strMsg = "WARNING: " + strMsg
        print(strMsg)
        self._iWarningCount+=1
        return "#WARNING: " + strMsg + "\n"
    def emitError(self, strMsg):
        print(strMsg)
        self._iErrorCount+=1
        return "#ERROR: " + strMsg + "\n"
    def euler3ToDeg(self, e3):
        #math.radians(45.0)
        eul = Euler((0.0, 0.0, 0.0), 'XYZ')
        eul.x = math.degrees(e3.x)
        eul.y = math.degrees(e3.y)
        eul.z = math.degrees(e3.z)
        return eul

        
        
###################################################        
###################################################
###################################################        

def getArgs():
    # get the args passed to blender after "--", all of which are ignored by
    # blender so scripts may receive their own arguments
    argv = sys.argv

    if "--" not in argv:
        argv = []  # as if no args are passed
    else:
        argv = argv[argv.index("--") + 1:]  # get all args after "--"

    # When --help or no args are given, print this help
    usage_text = (
            "type -outpath to get output path"
            )

    parser = argparse.ArgumentParser(description=usage_text)

    # Example utility, add some text and renders or saves it (with options)
    # Possible types are: string, int, long, choice, float and complex.
    parser.add_argument("-outpath", dest="outpath", type=str, required=False,
            help="Output Path")
    args = parser.parse_args(argv)  # In this example we wont use the args

    if not argv:
        parser.print_help()
        return args

    return args

def printExcept(e):
    print(str(e))
    exc_type, exc_obj, exc_tb = sys.exc_info()
    fname = os.path.split(exc_tb.tb_frame.f_code.co_filename)[1]
    print(fname, "line ", exc_tb.tb_lineno)
    print(traceback.format_exc())
    
try:

    outpath = os.path.abspath("./");
    p = None
    try:
        p = getArgs();
    except Exception as e:
        printExcept(e)
    
    if p == None:
        print("Error parsing args, outpath will default to " + outpath)
    elif not p.outpath:
        print("Error parsing args, outpath will default to " + outpath)
    else:
        outpath = p.outpath
        print("Modpath found: " + p.outpath);

    mob = MobExport(outpath)
    mob.export()
except Exception as e:
    #traceback.print_exc()
    print("$2Error$2: Failed to export model: " )
    printExcept(e)



###################################################
###################################################
###################################################














