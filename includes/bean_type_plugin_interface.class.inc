<?php

/**
 * @file
 * Bean classes and plugin interface
 */

/**
 * Interface for Plugin Classes
 */
interface BeanTypePluginInterface {

  /**
   * Constructor
   *
   * Plugin info should be called using bean_fetch_plugin_info().
   *
   * @abstract
   *
   * @param  $plugin_info
   *   Array of information from the plugin.
   */
  public function __construct($plugin_info);

  /**
   * Get the plugin info.
   *
   * If you pass in a key, then the key from the plugin info will be returned
   * otherwise the entire plugin will be returned
   *
   * @abstract
   *
   * @param  $key
   * @return array of plugin info for value froma key
   */
  public function getInfo($key = NULL);

  /**
   * Build the URL string for the plugin
   *
   * @abstract
   *
   * @return url
   */
  public function buildURL();

  /**
   * Get the label name of the plugin
   *
   * @abstract
   *
   * @return string
   *   Label of the type
   */
  public function getLabel();

  /**
   * Get the description of the plugin
   *
   * @abstract
   *
   * @return string
   *   Description of the type.
   */
  public function getDescription();

  /**
   * Set the Bean object for later use
   *
   * @abstract
   *
   * @param Bean $bean
   */
  public function setBean($bean);

  /**
   * Is this type editable?
   *
   * @abstract
   *
   * @return boolean
   */
  public function isEditable();

  /**
   * Define the form values and their defaults
   */
  public function values();

  /**
   * The Plugin Form
   *
   * The Bean object will be either loaded from the database or filled with the defaults.
   *
   * @param $bean
   * @param $form
   * @param $form_state
   * @return array
   *   form array
   */
  public function form($bean, $form, &$form_state);

  /**
   * Validate function for bean type form
   *
   * @abstract
   *
   * @param  $values
   * @param $form_state
   */
  public function validate($values, &$form_state);

  /**
   * React to a bean being saved
   *
   * @abstract
   *
   * @param Bean $bean
   */
  public function submit(Bean $bean);

  /**
   * Return the block content.
   *
   * @abstract
   *
   * @param Bean $bean
   *   The bean object being viewed.
   * @param array $content
   *   The default content array created by Entity API.  This will include any
   *   fields attached to the entity.
   * @param string $view_mode
   *   The view mode passed into $entity->view().
   *
   * @param string|null $langcode
   *
   * @return
   *   Return a renderable content array.
   */
  public function view($bean, $content, $view_mode = 'default', $langcode = NULL);
}
