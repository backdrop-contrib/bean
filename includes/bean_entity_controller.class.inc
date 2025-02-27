<?php

/**
 * @file
 * Bean entity controller class.
 */
class BeanEntityController extends EntityPlusControllerExportable {
  /**
 *
 */
  protected function setPlugin(Bean $bean) {
    static $plugins = array();

    if (empty($plugins[$bean->type])) {
      $plugins[$bean->type] = bean_load_plugin_class($bean->type);
      $bean->loadUp($plugins[$bean->type]);
    }
  }

  /**
 *
 */
  protected function attachLoad(&$queried_entities, $revision_id = FALSE) {
    foreach ($queried_entities as $entity) {
      $this->setPlugin($entity);
    }
    parent::attachLoad($queried_entities, $revision_id);
  }

  /**
 *
 */
  public function delete($ids, DatabaseTransaction $transaction = NULL) {
    $beans = $ids ? $this->load($ids) : array();

    $deltas = array();
    foreach ($beans as $bean) {
      if ($bean->id()) {
        field_attach_delete('bean', $bean);

        $deltas[] = $bean->id();
      }
    }

    // @todo: Shortcutting around this section for now. Is it still required
    //  in Backdrop, given that custom blocks are now in configuration?
    if (0 && !empty($deltas)) {
      try {
        // @see block_custom_block_delete_submit()
        if (module_exists('block')) {
          db_delete('block')
            ->condition('module', 'bean')
            ->condition('delta', $deltas)
            ->execute();
          db_delete('block_role')
            ->condition('module', 'bean')
            ->condition('delta', $deltas)
            ->execute();

          // @see node_form_block_custom_block_delete_submit()
          db_delete('block_node_type')
            ->condition('module', 'bean')
            ->condition('delta', $deltas)
            ->execute();
        }
      }
      catch (Exception $e) {
        if (isset($transaction)) {
          $transaction->rollback();
        }
        watchdog_exception('bean', $e);
        throw $e;
      }
    }
    parent::delete($ids, $transaction);
  }

  /**
   * Overridden to add Rules integration back in.
   */
  public function invoke($hook, $entity) {
    parent::invoke($hook, $entity);

    // Invoke rules.
    if (module_exists('rules')) {
      rules_invoke_event($this->entityType() . '_' . $hook, $entity);
    }
  }

  /**
   * Implements EntityPlusControllerExportable::resetCache().
   */
  public function resetCache(array $ids = NULL) {
    bean_reset(TRUE);
    parent::resetCache($ids);
  }

  /**
 *
 */
  protected function saveRevision($entity) {
    global $user;

    $org_uid = $entity->uid;
    $entity->uid = $user->uid;
    $return = parent::saveRevision($entity);
    $entity->uid = $org_uid;

    return $return;
  }
}
