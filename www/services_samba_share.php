#!/usr/local/bin/php
<?php
/*
	services_samba_share.php
	Modified for XHTML by Daisuke Aoyama (aoyama@peach.ne.jp)
	Copyright (C) 2010-2012 Daisuke Aoyama <aoyama@peach.ne.jp>.
	All rights reserved.

	Copyright (C) 2006-2010 Volker Theile (votdev@gmx.de)
	All rights reserved.

	part of FreeNAS (http://www.freenas.org)
	Copyright (C) 2005-2012 Olivier Cochard <olivier@freenas.org>.
	All rights reserved.
	
	Based on m0n0wall (http://m0n0.ch/wall)
	Copyright (C) 2003-2006 Manuel Kasper <mk@neon1.net>.
	All rights reserved.
	
	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions are met:
	
	1. Redistributions of source code must retain the above copyright notice,
	   this list of conditions and the following disclaimer.
	
	2. Redistributions in binary form must reproduce the above copyright
	   notice, this list of conditions and the following disclaimer in the
	   documentation and/or other materials provided with the distribution.
	
	THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
	INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
	AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
	AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
	OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
	SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
	INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
	CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
	ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
	POSSIBILITY OF SUCH DAMAGE.
*/
require("auth.inc");
require("guiconfig.inc");

$pgtitle = array(gettext("Services"), gettext("CIFS/SMB"), gettext("Shares"));

if ($_POST) {
	$pconfig = $_POST;

	if ($_POST['apply']) {
		$retval = 0;
		if (!file_exists($d_sysrebootreqd_path)) {
			$retval |= updatenotify_process("smbshare", "smbshare_process_updatenotification");
		  config_lock();
			$retval |= rc_update_service("samba");
			$retval |= rc_update_service("mdnsresponder");
			config_unlock();
		}
		$savemsg = get_std_save_message($retval);
		if (0 == $retval) {
			updatenotify_delete("smbshare");
		}
	}
}

if (!is_array($config['samba']['share']))
	$config['samba']['share'] = array();

array_sort_key($config['samba']['share'], "name");
$a_share = &$config['samba']['share'];

if ($_GET['act'] === "del") {
	updatenotify_set("smbshare", UPDATENOTIFY_MODE_DIRTY, $_GET['uuid']);
	header("Location: services_samba_share.php");
	exit;
}

function smbshare_process_updatenotification($mode, $data) {
	global $config;

	$retval = 0;

	switch ($mode) {
		case UPDATENOTIFY_MODE_NEW:
		case UPDATENOTIFY_MODE_MODIFIED:
			break;
		case UPDATENOTIFY_MODE_DIRTY:
			$cnid = array_search_ex($data, $config['samba']['share'], "uuid");
			if (FALSE !== $cnid) {
				unset($config['samba']['share'][$cnid]);
				write_config();
			}
			break;
	}

	return $retval;
}
?>
<?php include("fbegin.inc");?>
<table width="100%" border="0" cellpadding="0" cellspacing="0">
  <tr>
    <td class="tabnavtbl">
      <ul id="tabnav">
				<li class="tabinact"><a href="services_samba.php"><span><?=gettext("Settings");?></span></a></li>
				<li class="tabact"><a href="services_samba_share.php" title="<?=gettext("Reload page");?>"><span><?=gettext("Shares");?></span></a></li>
      </ul>
    </td>
  </tr>
  <tr> 
    <td class="tabcont">
      <form action="services_samba_share.php" method="post">
        <?php if ($savemsg) print_info_box($savemsg); ?>
        <?php if (updatenotify_exists("smbshare")) print_config_change_box();?>
        <table width="100%" border="0" cellpadding="0" cellspacing="0">
          <tr>
          	<td width="30%" class="listhdrlr"><?=gettext("Path");?></td>
            <td width="20%" class="listhdrr"><?=gettext("Name");?></td>
            <td width="20%" class="listhdrr"><?=gettext("Comment");?></td>
            <td width="10%" class="listhdrr"><?=gettext("Browseable");?></td>
            <td width="10%" class="listhdrr"><?=gettext("Guest");?></td>
            <td width="10%" class="list"></td>
          </tr>
  			  <?php foreach($a_share as $sharev):?>
  			  <?php $notificationmode = updatenotify_get_mode("smbshare", $sharev['uuid']);?>
          <tr>
          	<td class="listlr"><?=htmlspecialchars($sharev['path']);?>&nbsp;</td>
            <td class="listr"><?=htmlspecialchars($sharev['name']);?>&nbsp;</td>
            <td class="listr"><?=htmlspecialchars($sharev['comment']);?>&nbsp;</td>
            <td class="listbg"><?=htmlspecialchars(isset($sharev['browseable'])?gettext("Yes"):gettext("No"));?></td>
            <td class="listbg"><?=htmlspecialchars(isset($sharev['guest'])?gettext("Yes"):gettext("No"));?></td>
            <?php if (UPDATENOTIFY_MODE_DIRTY != $notificationmode):?>
            <td valign="middle" nowrap="nowrap" class="list">
              <a href="services_samba_share_edit.php?uuid=<?=$sharev['uuid'];?>"><img src="e.gif" title="<?=gettext("Edit share");?>" border="0" alt="<?=gettext("Edit share");?>" /></a>
              <a href="services_samba_share.php?act=del&amp;uuid=<?=$sharev['uuid'];?>" onclick="return confirm('<?=gettext("Do you really want to delete this share?");?>')"><img src="x.gif" title="<?=gettext("Delete share");?>" border="0" alt="<?=gettext("Delete share");?>" /></a>
            </td>
            <?php else:?>
						<td valign="middle" nowrap="nowrap" class="list">
							<img src="del.gif" border="0" alt="" />
						</td>
						<?php endif;?>
          </tr>
          <?php endforeach;?>
          <tr> 
            <td class="list" colspan="5"></td>
            <td class="list"><a href="services_samba_share_edit.php"><img src="plus.gif" title="<?=gettext("Add share");?>" border="0" alt="<?=gettext("Add share");?>" /></a></td>
          </tr>
        </table>
        <?php include("formend.inc");?>
      </form>
    </td>
  </tr>
</table>
<?php include("fend.inc");?>
