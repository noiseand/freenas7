<?php
/*
	guiconfig.inc
	part of FreeNAS (http://www.freenas.org)
	Copyright (C) 2005-2008 Olivier Cochard-Labbe <olivier@freenas.org>.
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

/* make sure nothing is cached */
if (!$omit_nocacheheaders) {
	header("Expires: 0");
	header("Last-Modified: " . gmdate("D, d M Y H:i:s") . " GMT");
	header("Cache-Control: no-store, no-cache, must-revalidate");
	header("Cache-Control: post-check=0, pre-check=0", false);
	header("Pragma: no-cache");
}

/* parse the configuration and include all configuration functions */
require_once("config.inc");
require_once("functions.inc");
require_once("rc.inc");
require_once("wui.inc");

/* Set the current language */
system_language_load();

$d_staticmapsdirty_path = $g['varrun_path'] . "/staticmaps.dirty";
$d_staticroutesdirty_path = $g['varrun_path'] . "/staticroutes.dirty";
$d_hostsdirty_path = $g['varrun_path'] . "/hosts.dirty";
$d_fwupenabled_path = $g['varrun_path'] . "/fwup.enabled";
$d_firmwarelock_path = $g['varrun_path'] . "/firmware.lock";
$d_sysrebootreqd_path = $g['varrun_path'] . "/sysreboot.reqd";
$d_allowedipsdirty_path = $g['varrun_path'] . "/allowedips.dirty";

$d_mountdirty_path = $g['varrun_path'] . "/mount.dirty";
$d_diskdirty_path = $g['varrun_path'] . "/disk.dirty";
$d_gelidirty_path = $g['varrun_path'] . "/geli.dirty";
$d_iscsiinitdirty_path = $g['varrun_path'] . "/iscsiinit.dirty";
$d_iscsitargetdirty_path = $g['varrun_path'] . "/iscsitarget.dirty";
$d_rsyncclientdirty_path = $g['varrun_path'] . "/rsyncclient.dirty";
$d_rsynclocaldirty_path = $g['varrun_path'] . "/rsynclocal.dirty";
$d_raid_graid5_confdirty_path = $g['varrun_path'] . "/raid_graid5.conf.dirty";
$d_raid_gmirror_confdirty_path = $g['varrun_path'] . "/raid_gmirror.conf.dirty";
$d_raid_gconcat_confdirty_path = $g['varrun_path'] . "/raid_gconcat.conf.dirty";
$d_raid_gvinum_confdirty_path = $g['varrun_path'] . "/raid_gvinum.conf.dirty";
$d_raid_gstripe_confdirty_path = $g['varrun_path'] . "/raid_gstripe.conf.dirty";
$d_userconfdirty_path = $g['varrun_path'] . "/user.conf.dirty";
$d_groupconfdirty_path = $g['varrun_path'] . "/group.conf.dirty";
$d_smbconfdirty_path = $g['varrun_path'] . "/smb.conf.dirty";
$d_smbshareconfdirty_path = $g['varrun_path'] . "/smbshare.conf.dirty";
$d_upnpconfdirty_path = $g['varrun_path'] . "/upnp.conf.dirty";
$d_nfsconfdirty_path = $g['varrun_path'] . "/nfs.conf.dirty";
$d_landirty_path = $g['varrun_path'] . "/lan.dirty";
$d_packagesconfdirty_path = $g['varrun_path'] . "/packages.conf.dirty";
$d_cronconfdirty_path = $g['varrun_path'] . "/cron.conf.dirty";
$d_afpconfdirty_path = $g['varrun_path'] . "/afp.conf.dirty";
$d_rsyncdconfdirty_path = $g['varrun_path'] . "/rsyncd.conf.dirty";
$d_nfsconfdirty_path = $g['varrun_path'] . "/nfs.conf.dirty";
$d_smartconfdirty_path = $g['varrun_path'] . "/smart.conf.dirty";
$d_websrvconfdirty_path = $g['varrun_path'] . "/websrv.conf.dirty";
$d_rcconfdirty_path = $g['varrun_path'] . "/rc.conf.dirty";
$d_sysctldirty_path = $g['varrun_path'] . "/sysctl.dirty";

if (file_exists($d_firmwarelock_path)) {
	if (!$d_isfwfile) {
		header("Location: system_firmware.php");
		exit;
	} else {
		return;
	}
}

/* Reserverd login names */
$reservedlogin = array("root", "toor", "daemon", "operator", "bin", "tty", "kmem", "www", "nobody", "ftp", "sshd");

/* TCP flags */
$tcpflags = array("fin", "syn", "rst", "psh", "ack", "urg");

/* platforms that support firmware updating */
$fwupplatforms = array('net45xx', 'net48xx', 'embedded', 'wrap', 'full');

function do_input_validation($postdata, $reqdfields, $reqdfieldsn, $input_errors) {
	/* check for bad control characters */
	foreach ($postdata as $pn => $pd) {
		if (is_string($pd) && preg_match("/[\\x00-\\x08\\x0b\\x0c\\x0e-\\x1f]/", $pd)) {
			$input_errors[] = sprintf( gettext("The attribute '%s' contains invalid characters."), $pn);
		}
	}

	for ($i = 0; $i < count($reqdfields); $i++) {
		if (!isset($postdata[$reqdfields[$i]]) || ($postdata[$reqdfields[$i]] === "")) {
			$input_errors[] = sprintf( gettext("The attribute '%s' is required."), $reqdfieldsn[$i]);
		}
	}
}

/* Validate attribute type. */
function do_input_validation_type($postdata, $reqdfields, $reqdfieldsn, $reqdfieldst, $input_errors) {
	/* Validate type. */
	for ($i = 0; $i < count($reqdfields); $i++) {
		if (isset($postdata[$reqdfields[$i]]) && ($postdata[$reqdfields[$i]] !== "")) {
			$valid = false;
			$message = "";

			switch($reqdfieldst[$i]) {
				case 'string':				$valid = is_string($postdata[$reqdfields[$i]]);
															$message = sprintf( gettext("The attribute '%s' must be a string."), $reqdfieldsn[$i]);
															break;
				case 'numeric':				$valid = is_numeric($postdata[$reqdfields[$i]]);
															$message = sprintf( gettext("The attribute '%s' must be a number."), $reqdfieldsn[$i]);
															break;
				case 'numericint':		$valid = is_numericint($postdata[$reqdfields[$i]]);
															$message = sprintf( gettext("The attribute '%s' must be a number."), $reqdfieldsn[$i]);
															break;
				case 'ipaddr':				$valid = is_ipaddr($postdata[$reqdfields[$i]]);
															$message = sprintf( gettext("The attribute '%s' is no valid IP address."), $reqdfieldsn[$i]);
															break;
				case 'macaddr': 			$valid = is_macaddr($postdata[$reqdfields[$i]]);
															$message = sprintf( gettext("The attribute '%s' is no valid MAC address."), $reqdfieldsn[$i]);
															break;
				case 'subnet': 				$valid = is_subnet($postdata[$reqdfields[$i]]);
															$message = sprintf( gettext("The attribute '%s' is no valid subnet mask."), $reqdfieldsn[$i]);
															break;
				case 'domain': 				$valid = is_domain($postdata[$reqdfields[$i]]);
															$message = sprintf( gettext("The attribute '%s' contains invalid characters and is no valid domain name."), $reqdfieldsn[$i]);
															break;
				case 'hostname': 			$valid = is_hostname($postdata[$reqdfields[$i]]);
															$message = sprintf( gettext("The attribute '%s' contains invalid characters and is no valid host name."), $reqdfieldsn[$i]);
															break;
				case 'workgroup': 		$valid = is_workgroup($postdata[$reqdfields[$i]]);
															$message = sprintf( gettext("The attribute '%s' contains invalid characters and is no valid workgroup name."), $reqdfieldsn[$i]);
															break;
				case 'filemode': 			$valid = is_filemode($postdata[$reqdfields[$i]]);
															$message = sprintf( gettext("The attribute '%s' is no valid file mode mask."), $reqdfieldsn[$i]);
															break;
				case 'mtu': 					$valid = is_mtu($postdata[$reqdfields[$i]]);
															$message = sprintf( gettext("The attribute '%s' is invalid."), $reqdfieldsn[$i]);
															break;
				case 'port': 					$valid = is_port($postdata[$reqdfields[$i]]);
															$message = sprintf( gettext("The attribute '%s' is an invalid port number."), $reqdfieldsn[$i]);
															break;
				case 'password':			$valid = is_validpassword($postdata[$reqdfields[$i]]);
															$message = sprintf( gettext("The attribute '%s' contains the illegal character ':' and is no valid password."), $reqdfieldsn[$i]);
															break;
				case 'certificate':		$valid = is_valid_certificate($postdata[$reqdfields[$i]]);
															$message = sprintf( gettext("The attribute '%s' does not appear to be a valid certificate."), $reqdfieldsn[$i]);
															break;
				case 'privatekey':		$valid = is_valid_privatekey($postdata[$reqdfields[$i]], "RSA");
															$message = sprintf( gettext("The attribute '%s' does not appear to be a valid private key."), $reqdfieldsn[$i]);
															break;
				case 'privatedsakey':	$valid = is_valid_privatekey($postdata[$reqdfields[$i]], "DSA");
															$message = sprintf( gettext("The attribute '%s' does not appear to be a valid private %s key."), $reqdfieldsn[$i], "DSA");
															break;
				case 'alias':					$valid = is_validaliasname($postdata[$reqdfields[$i]]);
															$message = sprintf( gettext("The attribute '%s' may only consist of the characters a-z, A-Z, 0-9."), $reqdfieldsn[$i]);
															break;
			}
			if (!$valid) {
				$input_errors[] = $message;
			}
		}
	}
}

function do_input_validate_synctime($postdata, $input_errors) {
	$data = array();
	$data[] = array("field" => "minute", "all" => "all_mins", "text" => "minutes");
	$data[] = array("field" => "hour", "all" => "all_hours", "text" => "hours");
	$data[] = array("field" => "day", "all" => "all_days", "text" => "days");
	$data[] = array("field" => "month", "all" => "all_months", "text" => "months");
	$data[] = array("field" => "weekday", "all" => "all_weekdays", "text" => "week days");

	foreach ($data as $datak => $datav) {
		if (!$postdata[$datav['all']]) {
			if (!isset($postdata[$datav['field']]) || empty($postdata[$datav['field']])) {
				$input_errors[] = sprintf(gettext("You have to select at least one item for '%s'."), $datav['text']);
			}
		}
	}
}

function print_input_errors($input_errors) {
	echo "<p>\n";
	echo "  <div id='errorbox'>\n";
	echo "    <table border=\"0\" cellspacing=\"0\" cellpadding=\"4\" width=\"100%\">\n";
	echo "      <tr>\n";
	echo "        <td class=\"icon\" align=\"center\" valign=\"top\"><img src=\"/err.gif\"></td>\n";
	echo "        <td class=\"message\">\n";
	echo "          <p>" . gettext("The following input errors were detected").":\n";
	echo "            <ul>\n";
	foreach ($input_errors as $msg) {
		echo "              <li>{$msg}</li>\n";
	}
	echo "            </ul>\n";
	echo "          </p>\n";
	echo "        </td>\n";
	echo "      </tr>\n";
	echo "    </table>\n";
	echo "  </div>\n";
	echo "</p>\n";
}

function verify_gzip_file($fname) {
	$returnvar = mwexec("/usr/bin/gzip -t " . escapeshellarg($fname), true);
	if ($returnvar != 0)
		return 0;
	else
		return 1;
}

function print_core_box_np($type, $msg) {
	switch ($type) {
		case "info":
			$id = "infobox";
			$img = "exclam.gif";
			break;
		case "warning":
			$id = "warningbox";
			$img = "warn.gif";
			break;
		case "error":
			$id = "errorbox";
			$img = "err.gif";
			break;
	}

	echo "<div id='{$id}'>\n";
	echo "  <table border=\"0\" cellspacing=\"0\" cellpadding=\"4\" width=\"100%\">\n";
	echo "    <tr>\n";
	echo "      <td class=\"icon\" align=\"center\" valign=\"top\"><img src=\"/{$img}\"></td>\n";
	echo "      <td class=\"message\">{$msg}</td>\n";
	echo "    </tr>\n";
	echo "  </table>\n";
	echo "</div>\n";
}

function print_info_box_np($msg) {
	print_core_box_np("info", $msg);
}

function print_info_box($msg) {
	echo "<p>";
	print_core_box_np("info", $msg);
	echo "</p>";
}

function print_error_box_np($msg) {
	print_core_box_np("error", $msg);
}

function print_error_box($msg) {
	echo "<p>";
	print_core_box_np("error", $msg);
	echo "</p>";
}

function print_warning_box($msg) {
	echo "<p>";
	print_core_box_np("warning", $msg);
	echo "</p>";
}

function print_config_change_box() {
	$message = gettext("Apply changes");
	echo "<p>";
	print_info_box_np(gettext("The configuration has been changed.<br>You must apply the changes in order for them to take effect."));
	echo "<br/><input name=\"apply\" type=\"submit\" class=\"formbtn\" id=\"apply\" value=\"{$message}\">";
	echo "</p>";
}

function format_bytes($bytes) {
	if ($bytes >= 1073741824) {
		return sprintf("%.2f GB", $bytes/1073741824);
	} else if ($bytes >= 1048576) {
		return sprintf("%.2f MB", $bytes/1048576);
	} else if ($bytes >= 1024) {
		return sprintf("%.0f KB", $bytes/1024);
	} else {
		return sprintf("%d bytes", $bytes);
	}
}

function get_std_save_message($ok) {
	global $d_sysrebootreqd_path;

	if ($ok == 0) {
		if (file_exists($d_sysrebootreqd_path)) {
			return sprintf( gettext("The changes have been saved. You have to <a href=%s>reboot</a> the system for the changes to take effect."), "reboot.php");
		}
		else {
			return gettext("The changes have been applied successfully.");
		}
	} else {
		return sprintf( gettext("Error: The changes could not be applied (error code %s)."), $ok);
	}
}

function html_inputbox($ctrlname, $title, $value, $desc, $required = false, $size = 40, $readonly = false) {
	$ctrl = new HTMLEditBox($ctrlname, $title, $value, $desc, $size);
	$ctrl->SetRequired($required);
	$ctrl->SetReadOnly($readonly);
	$ctrl->Render();
}

function html_passwordbox($ctrlname, $title, $value, $desc, $required = false, $size = 20, $readonly = false) {
	$ctrl = new HTMLPasswordBox($ctrlname, $title, $value, $desc, $size);
	$ctrl->SetRequired($required);
	$ctrl->SetReadOnly($readonly);
	$ctrl->Render();
}

function html_passwordconfbox($ctrlname, $ctrlnameconf, $title, $value, $valueconf, $desc, $required = false, $size = 20, $readonly = false) {
	$ctrl = new HTMLPasswordConfBox($ctrlname, $ctrlnameconf, $title, $value, $valueconf, $desc, $size);
	$ctrl->SetRequired($required);
	$ctrl->SetReadOnly($readonly);
	$ctrl->Render();
}

function html_textarea($ctrlname, $title, $value, $desc, $required = false, $columns = 40, $rows = 5, $readonly = false) {
	$ctrl = new HTMLTextArea($ctrlname, $title, $value, $desc, $columns, $rows);
	$ctrl->SetRequired($required);
	$ctrl->SetReadOnly($readonly);
	$ctrl->Render();
}

function html_filechooser($ctrlname, $title, $value, $desc, $path, $required = false, $size = 60, $readonly = false) {
	$ctrl = new HTMLFileChooser($ctrlname, $title, $value, $desc, $size);
	$ctrl->SetRequired($required);
	$ctrl->SetReadOnly($readonly);
	$ctrl->SetPath($path);
	$ctrl->Render();
}

function html_combobox($ctrlname, $title, $value, $options, $desc, $required = false, $readonly = false, $onclick = "") {
	$ctrl = new HTMLComboBox($ctrlname, $title, $value, $options, $desc);
	$ctrl->SetRequired($required);
	$ctrl->SetReadOnly($readonly);
	$ctrl->SetJSonClick($onclick);
	$ctrl->Render();
}

function html_mountcombobox($ctrlname, $title, $value, $desc, $required = false, $readonly = false) {
	$ctrl = new HTMLMountComboBox($ctrlname, $title, $value, $desc);
	$ctrl->SetRequired($required);
	$ctrl->SetReadOnly($readonly);
	$ctrl->Render();
}

function html_timezonecombobox($ctrlname, $title, $value, $desc, $required = false, $readonly = false) {
	$ctrl = new HTMLTimeZoneComboBox($ctrlname, $title, $value, $desc);
	$ctrl->SetRequired($required);
	$ctrl->SetReadOnly($readonly);
	$ctrl->Render();
}

function html_languagecombobox($ctrlname, $title, $value, $desc, $required = false, $readonly = false) {
	$ctrl = new HTMLLanguageComboBox($ctrlname, $title, $value, $desc);
	$ctrl->SetRequired($required);
	$ctrl->SetReadOnly($readonly);
	$ctrl->Render();
}

function html_interfacecombobox($ctrlname, $title, $value, $desc, $required = false, $readonly = false) {
	$ctrl = new HTMLInterfaceComboBox($ctrlname, $title, $value, $desc);
	$ctrl->SetRequired($required);
	$ctrl->SetReadOnly($readonly);
	$ctrl->Render();
}

function html_checkbox($ctrlname, $title, $checked, $caption = "", $desc = "", $required = false, $onclick = "") {
	$ctrl = new HTMLCheckBox($ctrlname, $title, $checked, $caption, $desc);
	$ctrl->SetRequired($required);
	$ctrl->SetJSonClick($onclick);
	$ctrl->Render();
}

function html_separator() {
	$ctrl = new HTMLSeparator();
	$ctrl->Render();
}

function html_titleline($title) {
	$ctrl = new HTMLTitleLine($title);
	$ctrl->Render();
}

function html_titleline_checkbox($ctrlname, $title, $value, $caption, $onclick = "") {
	$ctrl = new HTMLTitleLineCheckBox($ctrlname, $title, $value, $caption);
	$ctrl->SetJSonClick($onclick);
	$ctrl->Render();
}

function html_text($ctrlname, $title, $text) {
	$ctrl = new HTMLText($ctrlname, $title, $text);
	$ctrl->Render();
}

function html_listbox($ctrlname, $title, $value, $options, $desc, $required = false, $readonly = false, $onclick = "") {
	$ctrl = new HTMLListBox($ctrlname, $title, $value, $options, $desc);
	$ctrl->SetRequired($required);
	$ctrl->SetReadOnly($readonly);
	$ctrl->SetJSonClick($onclick);
	$ctrl->Render();
}

function html_ipv4addrbox($ctrlname, $ctrlnamenetmask, $title, $value, $valuenetmask, $desc, $required = false, $readonly = false) {
	$ctrl = new HTMLIPv4AddressBox($ctrlname, $ctrlnamenetmask, $title, $value, $valuenetmask, $desc);
	$ctrl->SetRequired($required);
	$ctrl->SetReadOnly($readonly);
	$ctrl->Render();
}

function html_ipv6addrbox($ctrlname, $ctrlnamenetmask, $title, $value, $valuenetmask, $desc, $required = false, $readonly = false) {
	$ctrl = new HTMLIPv6AddressBox($ctrlname, $ctrlnamenetmask, $title, $value, $valuenetmask, $desc);
	$ctrl->SetRequired($required);
	$ctrl->SetReadOnly($readonly);
	$ctrl->Render();
}

// Update notification constants
define("UPDATENOTIFICATION_MODE_NEW", 0);
define("UPDATENOTIFICATION_MODE_MODIFIED", 1);
define("UPDATENOTIFICATION_MODE_DIRTY", 2);
define("UPDATENOTIFICATION_MODE_UNKNOWN", 3);

define("UPDATENOTIFICATION_FLAG_NEW", "+");
define("UPDATENOTIFICATION_FLAG_MODIFIED", "*");
define("UPDATENOTIFICATION_FLAG_DIRTY", "-");

// Set a notification.
// $file - File to be used
// $mode - Mode to be set
// $data - Data to be set
function ui_set_updatenotification($file, $mode, $data) {
	switch ($mode) {
		case UPDATENOTIFICATION_MODE_NEW:
			$data = UPDATENOTIFICATION_FLAG_NEW . "|{$data}\n";
			break;

		case UPDATENOTIFICATION_MODE_MODIFIED:
			$data = UPDATENOTIFICATION_FLAG_MODIFIED . "|{$data}\n";
			break;

		case UPDATENOTIFICATION_MODE_DIRTY:
			$data = UPDATENOTIFICATION_FLAG_DIRTY . "|{$data}\n";
			break;
	}

	file_put_contents($file, $data, FILE_APPEND | FILE_TEXT);
}

// Get notifications.
// $file - File to be processed
// Return array of notifications if successful, otherwise false.
function ui_get_updatenotification($file) {
	if (!file_exists($file))
		return false;

	// Read file
	$a_filedata = file($file);
	if (!is_array($a_filedata))
		return false;

	// Parse data
	$a_notification = array();
	foreach ($a_filedata as $filedatav) {
		// Parse data. Syntax should look like: 'mode|data'.
		if (preg_match("/^([".UPDATENOTIFICATION_FLAG_NEW.UPDATENOTIFICATION_FLAG_MODIFIED.UPDATENOTIFICATION_FLAG_DIRTY."])\|([\S]+)/", $filedatav, $matches)) {
			$notification = array();
			$notification['mode'] = array_search($matches[1], array(UPDATENOTIFICATION_FLAG_NEW, UPDATENOTIFICATION_FLAG_MODIFIED, UPDATENOTIFICATION_FLAG_DIRTY));
			$notification['data'] = $matches[2];

			$a_notification[] = $notification;
		}
	}

	return $a_notification;
}

// Process notifications.
// $file - File to be processed
// $function - Name of callback function. Function syntax: xyz($mode, $data)
// Return 0 if successful, otherwise 1.
function ui_process_updatenotification($file, $function) {
	$a_notification = ui_get_updatenotification($file);
	if (!is_array($a_notification))
		return 1;

	foreach ($a_notification as $notificationv) {
		if (0 != call_user_func($function, $notificationv['mode'], $notificationv['data']))
			return 1;
	}

	return 0;
}
// Remove notification file.
// $file - File to be removed 
function ui_cleanup_updatenotification($file) {
	if (file_exists($file))
		unlink($file);
}

// Get the notification mode.
// $file - File to be used
// $data - Data to searched for
// Returns the notification mode, otherwise UPDATENOTIFICATION_MODE_UNKNOWN
function ui_get_updatenotification_mode($file, $data, $matchwholeword = true) {
	$a_notification = ui_get_updatenotification($file);
	if (!is_array($a_notification))
		return UPDATENOTIFICATION_MODE_UNKNOWN;

	// Remove duplicate entries. Last inserted notification wins (Last in, first out).
	$a_notification = array_reverse(array_remove_duplicate(array_reverse($a_notification), "data"));

	// Process array in reverse order => Last in, first out
	foreach ($a_notification as $notificationv) {
		if (true === $matchwholeword) {
			if ($notificationv['data'] === $data)
				return $notificationv['mode'];
		} else {
			if (false !== strstr($notificationv['data'], $data))
				return $notificationv['mode'];
		}
	}

	return UPDATENOTIFICATION_MODE_UNKNOWN;
}

// Check if specified notification mode is set.
// $file - File to be used
// $mode - Mode to searched for
// Return 0 if successful, otherwise 1.
function ui_isset_updatenotification_mode($file, $mode) {
	$a_notification = ui_get_updatenotification($file);
	if (!is_array($a_notification))
		return 1;

	// Remove duplicate entries. Last inserted notification wins (Last in, first out).
	$a_notification = array_reverse(array_remove_duplicate(array_reverse($a_notification), "data"));

	// Process array in reverse order => Last in, first out
	foreach ($a_notification as $notificationv) {
		if ($notificationv['mode'] == $mode)
				return 0;
	}

	return 1;
}
?>