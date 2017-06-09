'''
Created on 16.05.2017

@author: kokr

'''

'''
This file defines the different editors to manipulate setting entries.
To create new editors inherit a class from the GeneralEdit class and add
a link to it in the SettingEditorContainer class.
'''

from PyQt5 import QtWidgets
from PyQt5.QtCore import Qt

'''
The WidgetListElement class is a simple structure to store information of the widgets of an editor.
'''
class WidgetListElement(object):
	def __init__(self):
		self.label = None
		self.widget = None
		
'''
The GeneralEdit class is the parent class for all editors.
'''
class GeneralEdit(object):
	def __init__(self, settingData, parentWidget, parent=None):
		self.widgetList = []
		self.valueWidget = None
		self.settingData = settingData
		self.readOnly = True
		self.parent = parent
		
	def setFocus(self):
		pass
	
	def notifyChange(self):
		if self.parent != None:
			self.parent.notifyChange()
	
	def setValueToSettingData(self):
		return None
		
'''
The LineEdit class defines a simple line editor including value type validation
'''
class LineEdit(GeneralEdit):
	def __init__(self, settingData, parentWidget, editable, parent=None):
		super(LineEdit, self).__init__(settingData, parentWidget, parent)
		self.readOnly = not editable
		
		element = WidgetListElement()
		element.widget = QtWidgets.QLabel(self.settingData.description, parentWidget)
		element.widget.setWordWrap(True)
		self.widgetList.append(element)
		
		element = WidgetListElement()
		element.label = QtWidgets.QLabel(self.settingData.name, parentWidget)
		element.widget = QtWidgets.QLineEdit(self.settingData.value, parentWidget)
		element.widget.textEdited.connect(self.checkChanged)
		element.widget.setReadOnly(not editable)
		self.valueWidget = element.widget
		self.widgetList.append(element)
		
	
	def checkChanged(self):
		if self.valueWidget != None:
			if self.valueWidget.text() != self.settingData.value:
				self.notifyChange()
	
	def setFocus(self, reason):
		self.valueWidget.setFocus(reason)
		self.valueWidget.selectAll()
	
	def setValueToSettingData(self):
		returnMessage = None
		value = self.valueWidget.text()
		varType = self.settingData.varType
		if varType == "integer" or varType == "int":
			try:
				int(value)
				self.settingData.value = value
			except ValueError:
				returnMessage = ['The value at "'+self.settingData.name+'" must be an integer.', self]
				self.valueWidget.setText(self.settingData.value)
		elif varType == "float":
			try:
				float(value)
				self.settingData.value = value
			except ValueError:
				returnMessage = ['The value at "'+self.settingData.name+'" must be a floating point number.', self]
				self.valueWidget.setText(self.settingData.value)
		else:
			self.settingData.value = value
		return returnMessage
		
'''
The SelectEdit class shows a drop down menu to select certain setting elements.
'''
class SelectEdit(GeneralEdit):
	def __init__(self, settingData, parentWidget, parent=None):
		super(SelectEdit, self).__init__(settingData, parentWidget, parent)
		self.readOnly = False
		
		element = WidgetListElement()
		element.widget = QtWidgets.QLabel(self.settingData.description, parentWidget)
		element.widget.setWordWrap(True)
		self.widgetList.append(element)
		
		element = WidgetListElement()
		element.label = QtWidgets.QLabel(self.settingData.name, parentWidget)
		element.widget = QtWidgets.QComboBox(parentWidget)
		element.widget.currentIndexChanged.connect(self.checkChanged)
		element.widget.addItems(self.settingData.editorOptions.split(","))
		element.widget.setCurrentIndex(element.widget.findData(self.settingData.value, Qt.DisplayRole))
		self.valueWidget = element.widget
		self.widgetList.append(element)
		
	def checkChanged(self):
		if self.valueWidget != None:
			if self.valueWidget.currentText() != self.settingData.value:
				self.notifyChange()
		
	def setFocus(self, reason):
		self.valueWidget.setFocus(reason)
	
	def setValueToSettingData(self):
		possibleOptions = self.settingData.editorOptions.split(",")
		selectedText = self.valueWidget.currentText()
		found = False
		for options in possibleOptions:
			if selectedText == options:
				self.settingData.value = selectedText
				found = True
				break
		if found:
			return None
		else:
			return ['The selected value ' + selectedText + 'does not fit to any possible option: "' + self.settingData.editorOptions + '"', self]

'''
The NoSettingsText class is a special "Editor" that shows a placeholder text, if no settings are available for this child.
'''
class NoSettingsText(GeneralEdit):
	def __init__(self, settingData, parentWidget, parent=None):
		super(NoSettingsText, self).__init__(settingData, parentWidget, parent)
		
		element = WidgetListElement()
		element.widget = QtWidgets.QLabel('This element has no settings. Select a child element or create one by clicking "Add".', parentWidget)
		self.widgetList.append(element)
		
'''
The TextShow class is a special "Editor" that just displays user defined text.
'''
class TextShow(GeneralEdit):
	def __init__(self, settingData, parentWidget, parent=None):
		super(TextShow, self).__init__(settingData, parentWidget, parent)
		
		element = WidgetListElement()
		element.widget = QtWidgets.QLabel(self.settingData.value + self.settingData.description, parentWidget)
		self.widgetList.append(element)
		
'''
The HeadLine class is a special "Editor" that thows a headline with the name of the child.
'''
class Headline(GeneralEdit):
	def __init__(self, text, parentWidget, parent=None):
		super(Headline, self).__init__(None, parentWidget, parent)
		
		element = WidgetListElement()
		element.widget = QtWidgets.QLabel(text, parentWidget)
		element.widget.setStyleSheet("font-weight: bold");
		self.widgetList.append(element)
'''
The HorizontalLine class is a special "Editor" that shows a horizontal line so seperate editors.
'''
class HorizontalLine(GeneralEdit):
	def __init__(self, settingData, parentWidget, parent=None):
		super(HorizontalLine, self).__init__(settingData, parentWidget, parent)
		
		element = WidgetListElement()
		element.widget = QtWidgets.QFrame(parentWidget)
		element.widget.setFrameShape(QtWidgets.QFrame.HLine)
		element.widget.setFrameShadow(QtWidgets.QFrame.Sunken)
		self.widgetList.append(element)
		
'''
The SettingEditorContainer class is used to insance needed editors and hold links to them.
'''
class SettingEditorContainer(object):
	def __init__(self, child, parentWidget, parent=None):
		settings = child.settings
		self.editorList = []
		realSettings = False
		self.parent = parent
		
		self.editorList.append(Headline(child.name, parentWidget, self))
		self.editorList.append(HorizontalLine(None, parentWidget, self))
		if len(settings) != 0:
			for item in settings:
				
				#item.editor is the value at <editor> of a setting element in the plugin content xml file
				
				if item.editor == "lineedit":
					self.editorList.append(LineEdit(item, parentWidget, True, self))
					
				elif item.editor == "lineedit-readonly":
					self.editorList.append(LineEdit(item, parentWidget, False, self))
				
				elif item.editor == "select":
					self.editorList.append(SelectEdit(item, parentWidget, self))
					
				elif item.editor == "textshow":
					self.editorList.append(TextShow(item, parentWidget, self))
				
				else:
					item.editor = "invisible"
				
				if item.editor != "invisible" and item.editor != "textshow":
					realSettings = True
				
				if item.editor != "invisible":
					self.editorList.append(HorizontalLine(item, parentWidget, self))
			
			if not realSettings:
				self.editorList.append(NoSettingsText(None, parentWidget, self))
		else:
			self.editorList.append(NoSettingsText(None, parentWidget, self))

	def notifyChange(self):
		if self.parent != None:
			self.parent.setUnsavedContent(True)

'''
The EditorProbe class is used to check the values of an imported config file by the editor own validate methods
'''
class EditorProbe(object):
	def __init__(self):
		self.name = ""
		self.settings = []
		self.testEditorContainer = None
		self.editor = None
		
		
	def isReadOnly(self, settingEntry):
		self.recreateEditorContainer(settingEntry)
		if self.editor != None:
			return self.editor.readOnly
		else:
			return True

	def isValidValue(self, settingEntry):
		self.recreateEditorContainer(settingEntry)
		if self.editor == None:
			return None
		else:
			return self.editor.setValueToSettingData()
		
	
	def recreateEditorContainer(self, settingEntry):
		if len(self.settings) == 0:
			self.settings.append(settingEntry)
		else:
			self.settings[0] = settingEntry
		self.testEditorContainer = SettingEditorContainer(self, None)
		found = False
		for editorItem in self.testEditorContainer.editorList:
			if editorItem.settingData is self.settings[0]:
				self.editor = editorItem
				found = True
				break
		if not found:
			self.editor = None